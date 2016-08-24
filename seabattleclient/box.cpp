#include "box.h"
#include "map.h"

QWidget * SeaBattleClient::Box::pictureMain;
std::function<void(int, int, int)> SeaBattleClient::Box::boxClicked;

SeaBattleClient::Box::Box(QWidget *parent) :
    QPushButton{parent}
{
    setAutoFillBackground(true);
    QObject::connect(this, SIGNAL(clicked(bool)), this, SLOT(fire()));
}

void SeaBattleClient::Box::init(int boxPlace, int boxRow, int boxCol)
{
    place = boxPlace;
    row = boxRow;
    col = boxCol;
    int oceanPaddingX = place % 2 * (Map::mapSize + 1) * iconSize + iconSize;
    int oceanPaddingY = place / 2 * (Map::mapSize + 1) * iconSize + iconSize;

    int x = oceanPaddingX + col * (iconSize + 1);
    int y = oceanPaddingY + row * (iconSize + 1);

    resize(iconSize, iconSize);
    move(x, y);
}

SeaBattleClient::Box *SeaBattleClient::Box::create(SeaBattleClient::EnumIcons icon)
{
    switch(icon) {
    case EnumIcons::fail: return new FailBox{pictureMain};
    case EnumIcons::kill: return new KillBox{pictureMain};
    case EnumIcons::none: return new NoneBox{pictureMain};
    case EnumIcons::ship: return new ShipBox{pictureMain};
    case EnumIcons::unkn: return new UnknBox{pictureMain};
    case EnumIcons::fire: return new FireBox{pictureMain};
    }

    return new UnknBox{pictureMain};
}

void SeaBattleClient::Box::fire()
{
    boxClicked(place, row, col);
}

SeaBattleClient::NoneBox::NoneBox(QWidget *parent) :
    Box{parent}
{
    QPalette pal;
    pal.setBrush(backgroundRole(), QBrush(QPixmap(":/none.png")));
    setPalette(pal);
}

SeaBattleClient::UnknBox::UnknBox(QWidget *parent) :
    Box{parent}
{
    QPalette pal;
    pal.setBrush(backgroundRole(), QBrush(QPixmap(":/unkn.png")));
    setPalette(pal);
}

SeaBattleClient::ShipBox::ShipBox(QWidget *parent) :
    Box{parent}
{
    QPalette pal;
    pal.setBrush(backgroundRole(), QBrush(QPixmap(":/ship.png")));
    setPalette(pal);
}

SeaBattleClient::KillBox::KillBox(QWidget *parent) :
    Box{parent}
{
    QPalette pal;
    pal.setBrush(backgroundRole(), QBrush(QPixmap(":/kill.png")));
    setPalette(pal);
}

SeaBattleClient::FailBox::FailBox(QWidget *parent) :
    Box{parent}
{
    QPalette pal;
    pal.setBrush(backgroundRole(), QBrush(QPixmap(":/fail.png")));
    setPalette(pal);
}

SeaBattleClient::FireBox::FireBox(QWidget *parent) :
    Box{parent}
{
    QPalette pal;
    pal.setBrush(backgroundRole(), QBrush(QPixmap(":/fire.png")));
    setPalette(pal);
}
