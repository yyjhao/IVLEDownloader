/*
 * Main class to interate with IVLE
 * By Yao Yujian
 */

#ifndef IVLEFETCHER_H
#define IVLEFETCHER_H

#include <QObject>
#include <QtXml/QDomDocument>
#include <QtNetwork>
#include "json.h"
#include <QFile>
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
    explicit IVLEFetcher(QString, QString, QObject *parent = 0);
    QString username();
    int remainingFiles();


signals:
    void statusUpdate(fetchingState);
    void tokenUpdated(const QString& token);
    void error(fetchingError);
    void fileDownloaded(const QString &filename);
    
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
    void download();

    QString token;
    QDir path;
    QString _username;
    QNetworkAccessManager* manager;
    QVariantMap courses;
    int currentWebBinFetching;
    QVariantMap toDownload;
    int numOfFiles;
    //set parents of downloader and replies to this so that we can easily terminate a downloading session.
    QObject *session;
    QTimer *timer;
};

#endif // IVLEFETCHER_H
