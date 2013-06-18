#ifndef EXTERNALPAGEPARSER_H
#define EXTERNALPAGEPARSER_H

#include <QObject>
#include <QVariant>
#include <QtNetwork>
#include <QtWebkit>
#include "promise.h"

class ExternalPageParser : public QObject
{
    Q_OBJECT
    typedef QMap<QString, QMap<QString, QPair<QUrl, QString>>> Config;
public:
    explicit ExternalPageParser(const QVariantMap&, QObject *parent = 0);

    Promise* fetchFileInfo(const QStringList&);
    
signals:
    
public slots:
    void setConfig(const Config&);

private:
    Config config;
    QNetworkAccessManager * manager;
    Promise* process(const QString& course, const QString& dir, const QPair<QString, QString> info);
};

#endif // EXTERNALPAGEPARSER_H
