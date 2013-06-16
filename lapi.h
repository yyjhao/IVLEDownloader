#ifndef LAPI_H
#define LAPI_H

#include <QObject>
#include <QtNetwork>
#include "promise.h"

class Lapi : public QObject
{
    Q_OBJECT
public:
    explicit Lapi(QString token = "", QObject *parent = 0);
    Promise* validate();
    Promise* fetchUserInfo();
    Promise* fetchWorkbin(const QString &id);
    Promise* fetchAnnouncement();
    Promise* fetchModules();

    QString getWorkBinDownloadUrl(const QString&);

    void setToken(const QString&);

    QNetworkAccessManager* getManager();
    
public slots:
    

private:
    QString token;
    QNetworkAccessManager* manager;
    QNetworkRequest genRequest(const QString& service, const QString& additionalQuery = QString(""));
    void ajaxGet(const QNetworkRequest& re,
                 const std::function<void(QVariant)>&,
                 const std::function<void(QNetworkReply::NetworkError)>&);
    Promise* ajaxGet(const QNetworkRequest& re);
};

#endif // LAPI_H
