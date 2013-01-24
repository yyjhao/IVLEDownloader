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
#include "downloader.h"

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
    int remainingFiles();
    void setMaxFileSize(double);
    void setIgnoreUploadable(bool);
    void setExtraDownloads(QVariantMap&);

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
    void fetchModules();

private slots:
    void gotReply(QNetworkReply*);
    void updateDownload();

private:
    void validate();
    void fetchUserInfo();
    void buildDirectoriesAndDownloadList();
    void fetchWorkbins();
    void fetchWorkBin();
    void fetchAnnouncement();
    void fetchExtras();
    void download();
    QVariantMap mergeFileSystems(const QVariantMap&, const QVariantMap&);
    QVariantMap mergeFiles(const QVariantMap&, const QVariantMap&);
    void processAnnouncements(QVariantList);
    void workbinReady();
    void extraReady();

    // remove all empty folders
    // returns the cleaned up file structure
    QVariantMap cleanFileSystem(const QVariantMap&);
    QVariantMap jsonToFolder(const QVariantMap&);
    void exploreFolder(QDir&, const QVariantMap&);

    QString token;
    QDir path;
    QString _username;
    QNetworkAccessManager* manager;
    QVariantMap courses;
    QVariantMap extras;
    int currentWebBinFetching;
    QVariantMap toDownload;
    int numOfFiles;
    double maxFileSize;
    bool ignoreUploadable;
    //set parents of downloader and replies to this so that we can easily terminate a downloading session.
    QObject *session;
    QTimer *timer;
    bool isWorkbinReady, isExtraReady;
    // url: (course name, folder (can be .), exec)
    QMap<QString, QMap<QString, QString> > extrasInfo;
    int extrasToFetch;
};

#endif // IVLEFETCHER_H
