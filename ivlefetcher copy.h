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
    complete
};

enum fetchingError{
    wrongUsername
};

class IVLEFetcher : public QObject
{
    Q_OBJECT
public:
    explicit IVLEFetcher(QString, QString, QObject *parent = 0);
    void start();
    QString username();
    int remainingFiles();


signals:
    void statusUpdate(fetchingState);
    void tokenUpdated(const QString& token);
    void error(fetchingError);
    
public slots:
    void setDirectory(const QString&);

private slots:
    void gotReply(QNetworkReply*);
    void updateDownload();

private:
    void validate();
    void fetchUserInfo();
    void fetchModules();
    void buildDirectoriesAndDownloadList();
    void fetchWorkbins();
    void fetchWorkBin();
    void beginDownload();
    void download();

    QString token;
    QDir path;
    QString _username;
    QNetworkAccessManager* manager;
    QVariantMap courses;
    int currentWebBinFetching;
    QVariantMap toDownload;
    QVariantMap::Iterator toDlIt;
    int numOfFiles;
};

#endif // IVLEFETCHER_H
