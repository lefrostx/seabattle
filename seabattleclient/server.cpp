#include "server.h"
#include <QNetworkRequest>
#include <QNetworkReply>

SeaBattleClient::Server::Server(const QString &serverUrl, QObject *parent) :
    QObject{parent}, url{serverUrl}
{
    accessManager = new QNetworkAccessManager(this);
    connect(accessManager, &QNetworkAccessManager::finished,
            this,          &Server::replyFinished);
}

void SeaBattleClient::Server::requestXML(const QString &data)
{
    accessManager->get(QNetworkRequest(QUrl(url + data)));
}

int SeaBattleClient::Server::getTagCount(const QString &tag) const
{
    return domDoc.elementsByTagName(tag).count();
}

void SeaBattleClient::Server::replyFinished(QNetworkReply *reply)
{
    QString xml{reply->readAll()};
    reply->deleteLater();

    if (!domDoc.setContent(xml)) {
        emit receiveError();
        return;
    }
    parseXml();
}

template<typename T>
inline T SeaBattleClient::Server::getElementByTag(const QString &tag, int index) const
{
    return T{domDoc.elementsByTagName(tag).item(index).toElement().text()};
}

namespace SeaBattleClient {

    template<>
    inline int Server::getElementByTag<int>(const QString &tag, int index) const
    {
        return getElementByTag<QString>(tag, index).toInt();
    }

    template<>
    inline long long Server::getElementByTag<long long>(const QString &tag, int index) const
    {
        return getElementByTag<QString>(tag, index).toLongLong();
    }

}

void SeaBattleClient::ServerInfo::request()
{
    requestXML("?do=info");
}

void SeaBattleClient::ServerInfo::parseXml()
{
    status     = getElementByTag<QString>   ("status");
    ships      = getElementByTag<int>       ("ships");
    fireTicks  = getElementByTag<long long> ("fire_ticks");
    coolTicks  = getElementByTag<long long> ("cool_ticks");
    mapSize    = getElementByTag<int>       ("map_size");
    oceans     = getElementByTag<int>       ("oceans");
    freeOcean  = getElementByTag<int>       ("free_ocean");
    winOcean   = getElementByTag<int>       ("win_ocean");

    emit infoReceived();
}

void SeaBattleClient::ServerShip::request(int oceanIndex, const matrix<EnumShips> &ships)
{
    requestXML("?do=ship&ocean=" + QString::number(oceanIndex) + extractCoords(ships));
}

void SeaBattleClient::ServerShip::parseXml()
{
    ocean   = getElementByTag<int>      ("ocean");
    result  = getElementByTag<QString>  ("result");
    error   = getElementByTag<QString>  ("error");

    emit shipReceived();
}

QString SeaBattleClient::ServerShip::extractCoords(const matrix<SeaBattleClient::EnumShips> &ships) const
{
    QString coords;
    int shipIndex{1};

    for (int row{}; row < Map::mapSize; ++row)
        for (int col{}; col < Map::mapSize; ++col)
            if (ships(row, col) == EnumShips::ship) {
                coords += "&row" + QString::number(shipIndex) + '=' + QString::number(row) +
                          "&col" + QString::number(shipIndex) + '=' + QString::number(col);
                ++shipIndex;
            }

    return coords;
}

void SeaBattleClient::ServerFire::request(int fromOceanP, int toOceanP, int toRowP, int toColP)
{
    requestXML("?do=fire&from_ocean=" + QString::number(fromOceanP) +
                         "&to_ocean=" + QString::number(toOceanP) +
                           "&to_row=" + QString::number(toRowP) +
                           "&to_col=" + QString::number(toColP));
}

void SeaBattleClient::ServerFire::parseXml()
{
    fromOcean   = getElementByTag<int>      ("from_ocean");
    toOcean     = getElementByTag<int>      ("to_ocean");
    fromRow     = getElementByTag<int>      ("from_row");
    fromCol     = getElementByTag<int>      ("from_col");
    toRow       = getElementByTag<int>      ("to_row");
    toCol       = getElementByTag<int>      ("to_col");
    result      = getElementByTag<QString>  ("result");
    error       = getElementByTag<QString>  ("error");

    emit fireReceived(fromOcean, toOcean, toRow, toCol);
}

void SeaBattleClient::ServerGame::request()
{
    requestXML("?do=game&tick=" + QString::number(lastTick));
}

void SeaBattleClient::ServerGame::parseXml()
{
    status      = getElementByTag<QString>      ("status");
    lastTick    = getElementByTag<long long>    ("now_tick");

    actions.clear();
    int count{getTagCount("list")};

    for (int i{}; i < count; ++i) {
        Action record;

        record.tick         = getElementByTag<long long>    ("tick",        i);
        record.action       = getElementByTag<QString>      ("action",      i);
        record.fromOcean    = getElementByTag<int>          ("from_ocean",  i);
        record.toOcean      = getElementByTag<int>          ("to_ocean",    i);
        record.toRow        = getElementByTag<int>          ("to_row",      i);
        record.toCol        = getElementByTag<int>          ("to_col",      i);

        actions.push_back(record);
    }

    emit gameReceived();
}
