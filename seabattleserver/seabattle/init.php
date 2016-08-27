<?php
    
    $my_link = mysqli_connect("localhost", "videosharp", "videosharp");
    mysqli_select_db($my_link, "ship_game");

    function my_query($query)
    {
        global $my_link;

        $res = mysqli_query($my_link, $query);
        
        if (mysqli_errno($my_link) != 0) {
            echo "Error: " . mysqli_error($my_link) . "<br>";
            echo "Query: " . $query . "<br>";
            die();
        }

        return $res;
    }

    function select_row($query)
    {
        $res = my_query($query);
        if (mysqli_num_rows($res) == 0)
            return array();
        $row = mysqli_fetch_assoc($res);
        return $row;  
    }

    function select_scalar($query)
    {
        $res = my_query($query);
        if (mysqli_num_rows($res) == 0)
            return false;
        $row = mysqli_fetch_array($res, MYSQLI_NUM);
        return $row[0];  
    }

    function now_tick()
    {
        return intval(microtime(true) * 1000000);
    }
?>
