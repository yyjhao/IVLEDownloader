/*
 * Class used to download a file and write to hard disk continuously
 * By Yao Yujian
 */

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QtNetwork>
#include <QFile>
#include "promise.h"

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QString filePath, QNetworkReply*, QObject *parent = 0);
    ~Downloader();

    Promise* getPromise();
    
signals:
    void done(QString);
    void progress(qint64 now, qint64 total);
public slots:


private slots:
    void writeFile(qint64,qint64);
    void finished();

private:
    QFile* file;
    QNetworkReply* reply;
    Promise* promise;
    
};

#endif // DOWNLOADER_H
