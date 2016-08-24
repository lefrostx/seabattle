#ifndef SEABATTLECLIENT_SERVER_H
#define SEABATTLECLIENT_SERVER_H

#include <QNetworkAccessManager>
#include <QString>
#include <QtXml>
#include <QUrl>
#include "map.h"

namespace SeaBattleClient {
    class Server : public QObject {
        Q_OBJECT
    public:
        explicit Server(const QString &serverUrl, QObject *parent = 0);

    protected:
        void requestXML(const QString &data);
        virtual void parseXml() = 0;

        int getTagCount(const QString& tag) const;

        template<typename T>
        T getElementByTag(const QString &tag, int index = 0) const;

    signals:
        void receiveError();

    private slots:
        void replyFinished(QNetworkReply* reply);

    private:
        QNetworkAccessManager* accessManager{};
        QString url;
        QDomDocument domDoc;
    };

    class ServerInfo : public Server {
        Q_OBJECT
    public:
        explicit ServerInfo(const QString &serverUrl, QObject *parent = 0) :
            Server{serverUrl, parent} {}

        void request();

        QString     getStatus()     const {return status;}
        int         getShips()      const {return ships;}
        long long   getFireTicks()  const {return fireTicks;}
        long long   getCoolTicks()  const {return coolTicks;}
        int         getMapSize()    const {return mapSize;}
        int         getOceans()     const {return oceans;}
        int         getFreeOcean()  const {return freeOcean;}
        int         getWinOcean()   const {return winOcean;}

    protected:
        void parseXml() override;

    signals:
        void infoReceived();

    private:
        QString status;
        int ships;
        long long fireTicks;
        long long coolTicks;
        int mapSize;
        int oceans;
        int freeOcean;
        int winOcean;
    };

    class ServerShip : public Server {
        Q_OBJECT
    public:
        explicit ServerShip(const QString &serverUrl, QObject *parent = 0) :
            Server{serverUrl, parent} {}

        void request(int oceanIndex, const matrix<EnumShips>& ships);

        int     getOcean()  const {return ocean;}
        QString getResult() const {return result;}
        QString getError()  const {return error;}

    protected:
        void parseXml() override;

    signals:
        void shipReceived();

    private:
        QString extractCoords(const matrix<EnumShips>& ships) const;
        int ocean;
        QString result;
        QString error;
    };

    class ServerFire : public Server {
        Q_OBJECT
    public:
        explicit ServerFire(const QString &serverUrl, QObject *parent = 0) :
            Server{serverUrl, parent} {}

        void request(int fromOceanP, int toOceanP, int toRowP, int toColP);

        int     getFromOcean()  const {return fromOcean;}
        int     getToOcean()    const {return toOcean;}
        int     getFromRow()    const {return fromRow;}
        int     getFromCol()    const {return fromCol;}
        int     getToRow()      const {return toRow;}
        int     getToCol()      const {return toCol;}
        QString getResult()     const {return result;}
        QString getError()      const {return error;}

    protected:
        void parseXml() override;

    signals:
        void fireReceived(int, int, int, int);

    private:
        int fromOcean;
        int toOcean;
        int fromRow;
        int fromCol;
        int toRow;
        int toCol;
        QString result;
        QString error;
    };

    class ServerGame : public Server {
        Q_OBJECT
    public:
        struct Action {
            long long tick;
            QString action;
            int fromOcean;
            int toOcean;
            int toRow;
            int toCol;
        };

        explicit ServerGame(const QString &serverUrl, QObject *parent = 0) :
            Server{serverUrl, parent} {}

        void request();
        const   std::vector<Action> getActions()    const {return actions;}
        QString getStatus()                         const {return status;}

    protected:
        void parseXml() override;

    signals:
        void gameReceived();

    private:
        QString status;
        long long lastTick{};
        std::vector<Action> actions;
    };
    
}

#endif // SEABATTLECLIENT_SERVER_H
