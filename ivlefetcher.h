/*
 * Main class to interate with IVLE
 * By Yao Yujian
 */

#ifndef IVLEFETCHER_H
#define IVLEFETCHER_H

#include <QObject>
#include <QtXml/QDomDocument>
#include <QtNetwork>
#include <QFile>
#include <QJsonDocument>
#include <QWebPage>
#include <QWebFrame>
#include "downloader.h"
#include "lapi.h"
#include "externalpageparser.h"

enum fetchingState{
    gettingUserInfo,
    gottenUserInfo,
    gettingFileList,
    gottenFileList,
    gettingWebbinInfo,
    gottenWebbinInfo,
    downloading,
    remainingChange,
    complete,
    networkError,
    invalidToken
};

enum fetchingError{
    wrongUsername
};

class IVLEFetcher : public QObject
{
    Q_OBJECT
public:
    explicit IVLEFetcher(QString, ExternalPageParser*, QString, double, QObject *parent = 0);
    QString username();
    void setMaxFileSize(double);
    void setIgnoreUploadable(bool);
    void setExtraDownloads(const QVariantMap&);
    int remainingFiles();

signals:
    void statusUpdate(fetchingState);
    void tokenUpdated(const QString& token);
    void error(fetchingError);
    void fileDownloaded(const QString &filename);
    void gotUnreadAnnouncements(QVariantList);
    
public slots:
    void setDirectory(const QString&);
    void setToken(const QString&);
    void start();

private:
    void buildDirectoriesAndDownloadList();
    QVariantMap mergeFileSystems(const QVariantMap&, const QVariantMap&);
    QVariantMap mergeFiles(const QVariantMap&, const QVariantMap&);
    void processAnnouncements(QVariantList);
    QVariantMap toDownload;

    QVariantMap cleanFileSystem(const QVariantMap&);
    QVariantMap jsonToFolder(const QVariantMap&);
    void exploreFolder(QDir&, const QVariantMap&);

    QString token;
    QDir path;
    QString _username;
    QVariantMap courses;
    QVariantMap extras;
    int numOfFiles;

    double maxFileSize;
    bool ignoreUploadable;
    //set parents of downloader and replies to this so that we can easily terminate a downloading session.
    QObject *session;
    QTimer *timer;
    // url: (course name, folder (can be .), exec)
    Lapi* api;
    ExternalPageParser* parser;
};

#endif // IVLEFETCHER_H
