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
    networkError
};

enum fetchingError{
    wrongUsername
};

class IVLEFetcher : public QObject
{
    Q_OBJECT
public:
    explicit IVLEFetcher(QString, QVariantMap, QString, double, QObject *parent = 0);
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
    void download();
    QVariantMap mergeFileSystems(const QVariantMap&, const QVariantMap&);
    QVariantMap mergeFiles(const QVariantMap&, const QVariantMap&);
    void processAnnouncements(QVariantList);
    void parsePage(const QByteArray &content, const QString &course, const QString &folder, const QString &exec, QUrl baseUrl);
    QVariantMap resolveRelFileUrls(const QVariantMap&, const QUrl&);
    QVariantMap toDownload;

    QVariantMap cleanFileSystem(const QVariantMap&);
    QVariantMap jsonToFolder(const QVariantMap&);
    void exploreFolder(QDir&, const QVariantMap&);

    QString token;
    QDir path;
    QString _username;
    QVariantMap courses;
    QVariantMap extras;
    int currentWebBinFetching;
    int numOfFiles;

    double maxFileSize;
    bool ignoreUploadable;
    //set parents of downloader and replies to this so that we can easily terminate a downloading session.
    QObject *session;
    QTimer *timer;
    // url: (course name, folder (can be .), exec)
    QMap<QString, QMap<QString, QString> > extrasInfo;
    QMap<QString, QList<QString> > namedExtrasInfo;
    QSet<QString> allCourseNames;

    Lapi* api;
};

#endif // IVLEFETCHER_H
