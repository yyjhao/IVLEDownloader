#include "downloader.h"

Downloader::Downloader(QString fileName, QNetworkReply* reply, QObject *parent) :
    QObject(parent)
{
    file = new QFile(fileName + ".ivledl");
    if(file->exists()){
        file->remove();
    }
    file->open(QIODevice::WriteOnly);
    this->reply = reply;

    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(writeFile(qint64,qint64)));
    connect(reply,SIGNAL(finished()),this,SLOT(finished()));

    promise = new Promise(this);
}

Downloader::~Downloader(){
    delete file;
}

void Downloader::finished(){
    file->rename(file->fileName().replace(QString(".ivledl"),QString("")));
    file->close();
    emit done(reply->url().toString());
    promise->resolve(file->fileName());
    this->deleteLater();
}

void Downloader::writeFile(qint64 now, qint64 total){
    //qDebug()<<reply->size();
    file->write(reply->readAll());
}

Promise* Downloader::getPromise(){
    return promise;
}