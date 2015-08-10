#ifndef EXTERNALPAGEPARSER_H
#define EXTERNALPAGEPARSER_H

#include <QObject>
#include <QVariant>
#include <QtNetwork>
#include <QtWebKit>
#include <QWebPage>
#include <QWebFrame>
#include "promise.h"

class ExternalPageParser : public QObject
{
    Q_OBJECT
public:
    typedef QMap<QString, QMap<QString, QPair<QUrl, QString>>> Config;
    explicit ExternalPageParser(const Config&, QObject *parent = 0);

    Promise* fetchFileInfo(const QStringList&);
    
signals:
    
public slots:
    void setConfig(const Config&);

private:
    Config config;
    QNetworkAccessManager * manager;
    Promise* process(const QString& course, const QString& dir, const QPair<QUrl, QString>& info);

    static const QVariantMap resultListToMap(const QList<QString>&, const QUrl& base);
};

#endif // EXTERNALPAGEPARSER_H
