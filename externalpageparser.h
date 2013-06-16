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
public:
    explicit ExternalPageParser(const QVariantMap&, QObject *parent = 0);

    Promise* fetchFiles(const QStringList&);
    
signals:
    
public slots:
    void setConfig(const QVariantMap&);

private:
    QVariantMap config;
};

#endif // EXTERNALPAGEPARSER_H
