<?php
    include "init.php";

    $do = @$_GET["do"];
    switch ($do) {
        case "restart":  do_restart(); $xml = ""; break;
        case "info": $xml = do_info(); break;
        case "ship": $xml = do_ship(); break;
        case "fire": $xml = do_fire(); break;
        case "game": $xml = do_game(); break;
        default    : $xml = "<error>Invalid do command</error>"; break;
    }

    $xml = "<?xml version=\"1.0\"?>\n" . $xml;
    //echo "<textarea cols='60' rows='120'>$xml</textarea>";
    echo $xml;

    function do_restart()
    {
        my_query("TRUNCATE TABLE `ship`");
        my_query("TRUNCATE TABLE `game`");
        my_query("UPDATE `info` SET `status`='wait'");
    }

    function do_info()
    {
        $row = select_row(
            "SELECT `status`, `ships`, `fire_ticks`, `cool_ticks`,
                    `map_size`, `oceans`
               FROM `info`");

        extract($row);
        $free_ocean = "-1";
        
        if ($status == "stop")
            $free_ocean = "0";

        else if ($status == "wait") {
            for ($i = 0; $i < $oceans; $i++) {

                $query = "SELECT COUNT(*)
                            FROM `ship`
                           WHERE `ocean`= '$i'";

                if (select_scalar($query) == 0) {
                    $free_ocean = $i;
                    break;
                }
            }
        }
        $win_ocean = -1;

        if ($status == "stop") {
            $win_ocean = select_scalar(
                "SELECT `ocean`
                   FROM `ship`
                  WHERE `flag`<>'kill'
                  LIMIT 1");
        }

        $xml =
"<info>
    <status>$status</status>
    <ships>$ships</ships>
    <fire_ticks>$fire_ticks</fire_ticks>
    <cool_ticks>$cool_ticks</cool_ticks>
    <map_size>$map_size</map_size>
    <oceans>$oceans</oceans>
    <free_ocean>$free_ocean</free_ocean>
    <win_ocean>$win_ocean</win_ocean>
</info>";

        return $xml;
    }

    function do_ship()
    {
        $ocean = "";
        $result = "";
        $error = "unknown";

        // инициализация переменных настроек игры из таблицы info
        $rrow = select_row(
            "SELECT `status`, `ships`, `fire_ticks`, `cool_ticks`,
                    `map_size`, `oceans`
               FROM `info`");
        
        extract($rrow);

        do {

            if ($status == "stop") {
                do_restart();
                $status = "wait";
            }

            /* проверка текущего статуса игры: если игра не находится в режиме
             * ожидания ввода начального положения кораблей игроков, 
             * то выход с ошибкой err5 */
            if ($status != "wait") {
                $result = "err5";
                $error = "Ship command not applicable for current status $status";
                break;
            }

            // проверка на наличие в строке GET параметра ocean 
            if (!isset($_GET["ocean"])) {
                $result = "err0";
                $error = "Ocean not presented";
                break;
            }
              
            $ocean = intval($_GET["ocean"]);                   
            // проверка на наличие ошибки err0 - некорректный номер океана
            if ($ocean < 0 || $ocean >= $oceans) {
                $result = "err0";
                $error = "Invalid ocean number";
                break;
            }
            
            /* очистка текущего океана $ocean от всех имеющихся кораблей
             * для проверки наличия ошибок err2, err3, err4  */
            my_query("DELETE
                        FROM `ship`
                       WHERE `ocean`='" . addslashes($ocean) . "';");

            for ($i = 1; $i <= $ships; $i++) {
                // проверка на корректность координат в GET-запросе для err1
                if (!isset($_GET["row$i"]) || !isset($_GET["col$i"])) {
                    $result = "err1";
                    $error = "There are not ship number $i";
                    break;
                }
                $row = $_GET["row" . $i];
                $col = $_GET["col" . $i];
                
                // проверка на ошибку err4 - координаты кораблей дублируются
                if (select_scalar(
                    "SELECT COUNT(*)
                       FROM `ship`
                      WHERE `ocean`='$ocean'
                        AND `row` = '" . addslashes($row) . "'
                        AND `col` = '" . addslashes($col) . "'")) {
                    $result = "err4";
                    $error = "There are duplicated ships coords";
                    break;
                }
                my_query("INSERT INTO `ship`
                          (`ocean`, `row`, `col`, `flag`, `cool_tick`)
                          VALUES('$ocean',
                                 '" . addslashes($row) . "',
                                 '" . addslashes($col) . "',
                                 'ship', 0)");
            }

            if ($result != "")
                break;

            $ii = $ships + 1;
            if (isset($_GET["row$ii"]) || isset($_GET["col$ii"])) {
                $result = "err8";
                $error = "There must be only $ships ships";
                break;
            }

            // проверка на ошибку err1 - неверное количество кораблей
            $placed = select_scalar (
                "SELECT COUNT(*)
                   FROM `ship`
                  WHERE `ocean`='" . addslashes($ocean) . "'");

            if ($placed != $ships) {
                $result = "err1";
                $error = "Wrong count of ships, found $placed of $ships";
                break;
            }

            // проверка на ошибку err2 - выход за пределы поля
            $outmap = select_scalar(
                "SELECT COUNT(*)
                   FROM `ship`
                  WHERE `ocean`='" . addslashes($ocean) . "'
                    AND (`row` < 0 OR `row` >= '$map_size' OR 
                         `col` < 0 OR `col` >= '$map_size')");
            
            if ($outmap > 0) {
                $result = "err2";
                $error = "Coord of $outmap ships are out of map range";
                break;
            }

            // проверка на ошибку err3 - корабли соприкасаются
            for ($i = 1; $i <= $ships; $i++) {
                $row = $_GET["row" . $i];
                $col = $_GET["col" . $i];

                $nears = select_scalar (
                    "SELECT COUNT(*)
                       FROM `ship`
                      WHERE `ocean`='" . addslashes($ocean) . "'
                        AND (`row` BETWEEN $row-1 AND $row+1)
                        AND (`col` BETWEEN $col-1 AND $col+1)");
                
                if ($nears > 1) {
                    $result = "err3";
                    $error = "Ship $row,$col touch other ship";
                    break;
                }
            }

            if ($result != "")
                break;

            // ошибок не обнаружено, корабли успешно добавлены
            $result = "ok";
            $error = "Ships are placed";

            // если все игроки добавили корабли, то смена статуса игры на play
            $total_ships = select_scalar(
                "SELECT COUNT(*)
                   FROM `ship`
                ");
            if ($total_ships == $ships * $oceans) {
                my_query("UPDATE `info`
                             SET `status`='play'");
                my_query("TRUNCATE TABLE `game`");
            }

        } while (false);    

        if (!in_array($result, array("ok", "err5", "err0")))
            my_query("DELETE
                        FROM `ship`
                       WHERE `ocean`='" . addslashes($ocean) . "';");

        $xml =
"<ship>
    <ocean>$ocean</ocean>
    <result>$result</result>
    <error>$error</error>
</ship>";
        return $xml;
    }

    function do_fire()
    {
        $result = "";
        $from_ocean = "";
        $to_ocean = "";
        $from_row = "";
        $from_col = "";
        $to_row = "";
        $to_col = "";
        $error = "unknown";
        do {
            // инициализация переменных настроек игры из таблицы info
            $rrow = select_row(
                "SELECT `status`, `ships`, `fire_ticks`, `cool_ticks`,
                        `map_size`, `oceans`
                   FROM `info`");
        
            extract($rrow);

            // выход с ошибкой err5, если статус игры не равен play
            if ($status != "play") {
                $result = "err5";
                $error = "Fire command not applicable for current status $status";
                break;
            }

            // проверка на наличие в строке GET параметра from_ocean 
            if (!isset($_GET["from_ocean"])) {
                $result = "err0";
                $error = "from_ocean not presented";
                break;
            }

            // проверка на наличие в строке GET параметра to_ocean 
            if (!isset($_GET["to_ocean"])) {
                $result = "err0";
                $error = "to_ocean not presented";
                break;
            }
              
            $from_ocean = intval($_GET["from_ocean"]);          
            $to_ocean   = intval($_GET["to_ocean"]);                   
            // проверка на наличие ошибки err0 для from_ocean
            if ($from_ocean < 0 || $from_ocean >= $oceans) {
                $result = "err0";
                $error = "Invalid from_ocean number";
                break;
            }

            // проверка на наличие ошибки err0 для to_ocean
            if ($to_ocean  < 0 || $to_ocean  >= $oceans) {
                $result = "err0";
                $error = "Invalid to_ocean number";
                break;
            }         
            // проверка на наличие в строке GET параметров to_row, to_col
            if (!isset($_GET["to_row"]) || !isset($_GET["to_col"])) {
                $result = "err2";
                $error = "Fire coords not presented";
                break;
            }

            $to_row = intval(1 * $_GET["to_row"]);
            $to_col = intval(1 * $_GET["to_col"]);

            // если координаты вне игрового поля, то выход с ошибкой err2
            if ($to_row < 0 || $to_row >= $map_size ||
                $to_col < 0 || $to_col >= $map_size) {
                $result = "err2";
                $error = "Fire coords out of map range";
                break;
            }

            $rest_ships = select_scalar (
                "SELECT COUNT(*)
                  FROM `ship`
                 WHERE `ocean`='" . addslashes($from_ocean) . "'
                   AND `flag`<>'kill'");

            // если на океане $from_ocean все корабли подбиты, то выход с ошибкой err7
            if ($rest_ships == 0) {
                $result = "err7";
                $error = "All your ships are killed, no fire possible";
                break;
            }          

            $now_tick = now_tick();

            // обновить состояние кораблей - закончить перезарядку пушек
            my_query (
                "UPDATE `ship`
                    SET `flag`='ship'
                  WHERE `ocean`='" . addslashes($from_ocean) . "'
                    AND `flag`='cool'
                    AND `cool_tick`<='$now_tick'");

            $from_ship = select_row (
                "SELECT `row`, `col`
                   FROM `ship`
                  WHERE `ocean`='" . addslashes($from_ocean) . "'
                    AND `flag`='ship'
                  ORDER BY `row`, `col`
                  LIMIT 1");

            // если все пушки на кораблях перезаряжаются, то выход с ошибкой err6
            if (count($from_ship) == 0) {
                $result = "err6";
                $error = "All ships are cooled, please try again later";
                break;
            }

            $from_row = $from_ship["row"];
            $from_col = $from_ship["col"];

            /* установить состояние перезарядки и момент окончания перезарядки
             * пушки для выстрелившего корабля */
            my_query(
                "UPDATE `ship`
                    SET `flag`='cool',
                        `cool_tick`='" . ($now_tick+$cool_ticks) . "'
                  WHERE `ocean`='" . addslashes($from_ocean) . "'
                    AND `row`='$from_row'
                    AND `col`='$from_col'");

            $to_exist = select_scalar(
                "SELECT COUNT(*)
                   FROM `ship`
                  WHERE `ocean`='" . addslashes($to_ocean) . "'
                    AND `row`='$to_row'
                    AND `col`='$to_col'");

            // если выстрел успешный, то установить действие kill, иначе fail
            $action = ($to_exist > 0) ? 'kill' : 'fail';

            // если выстрел успешный, то обновить состояние корабля в таблице ship
            if ($action == 'kill') {
                my_query(
                    "UPDATE `ship`
                        SET `flag`='kill'
                      WHERE `ocean`='" . addslashes($to_ocean) . "'
                        AND `row`='$to_row'
                        AND `col`='$to_col'");
                
                $live_oceans = select_scalar(
                    "SELECT COUNT(DISTINCT `ocean`)
                       FROM `ship`
                      WHERE `flag`<>'kill'");
                // если остался только один "живой" океан, то остановить игру
                if ($live_oceans <= 1) {
                    my_query(
                        "UPDATE `info`
                            SET `status`='stop'");
                }
            }
            //добавить выстрел и его результат в таблицу game
            my_query(
                "INSERT
                   INTO `game`(`tick`,`action`,`from_ocean`,`to_ocean`,`to_row`, `to_col`)
                 VALUES ('$now_tick',
                         'fire',
                         '" . addslashes($from_ocean) . "',
                         '" . addslashes($to_ocean) . "',
                         '$to_row',
                         '$to_col')");
            
             my_query(
                "INSERT
                   INTO `game`(`tick`,`action`,`from_ocean`,`to_ocean`,`to_row`, `to_col`)
                 VALUES ('" . ($now_tick + $fire_ticks) . "',
                         '$action',
                         '" . addslashes($from_ocean) . "',
                         '" . addslashes($to_ocean) . "',
                         '$to_row',
                         '$to_col')");     
            
            $result = "ok";
            $error = "Fire to $to_ocean at $to_row,$to_col accepted";
            
        } while (false);

        $xml =
"<fire>
    <from_ocean>$from_ocean</from_ocean>
    <to_ocean>$to_ocean</to_ocean>
    <result>$result</result>
    <error>$error</error>
    <from_row>$from_row</from_row>
    <from_col>$from_col</from_col>
    <to_row>$to_row</to_row>
    <to_col>$to_col</to_col>
</fire>";
        return $xml;
    }
    
    function do_game()
    {
        $list_xml = "";
        $tick = (isset($_GET["tick"])) ? intval($_GET["tick"]) : 0;

        $now_tick = now_tick();
        $rrow = select_row("SELECT `status`
                              FROM `info`");
        extract($rrow);

        $res = my_query(
            "SELECT `tick`, `action`, `from_ocean`, `to_ocean`, `to_row`, `to_col`
               FROM `game`
              WHERE `tick` > '" . addslashes($tick) . "'
                AND `tick` <= '$now_tick'");

        while($row = mysqli_fetch_assoc($res)) {
            extract($row);
            $list_xml .= "
    <list>
        <tick>$tick</tick>
        <action>$action</action>
        <from_ocean>$from_ocean</from_ocean>
        <to_ocean>$to_ocean</to_ocean>
        <to_row>$to_row</to_row>
        <to_col>$to_col</to_col>
    </list>";
        }
        
        $xml =
"<game>
    <now_tick>$now_tick</now_tick>
    <status>$status</status>" .
        $list_xml . "  
</game>";

        return $xml;
    }
?>
