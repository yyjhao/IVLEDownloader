#include "ivlefetcher.h"
#include <QDebug>

IVLEFetcher::IVLEFetcher(QString token, QString dir, double maxfilesize, QObject *parent) :
    QObject(parent)
{
    this->token = token;
     this->path = QDir(dir);
    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(gotReply(QNetworkReply*)));
    session = new QObject(this);
    this->maxFileSize = maxfilesize;
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(fetchModules()));
}

void IVLEFetcher::start(){
    emit statusUpdate(gettingUserInfo);
    validate();
}

QVariantMap IVLEFetcher::jsonToFolder(const QVariantMap& map){
    QVariantList filelist = map.value("Files").toList();
    QVariantMap files,folder;
    for(int j = 0; j < filelist.count(); j++){
        QVariantMap file, fileJS = filelist[j].toMap();
        if(fileJS.value("FileSize").toDouble() < maxFileSize){
            //ignore files that are too big
            file.insert("name",fileJS.value("FileName"));
            file.insert("uploadTime",fileJS.value("UploadTime_js").toDate());
            files.insert(fileJS.value("ID").toString(),file);
        }
    }
    folder.insert("files",files);
    QVariantMap folders;
    QVariantList folderlist = map.value("Folders").toList();
    if(folderlist.count() > 0){
        for(int i = 0; i < folderlist.count(); i++){
            folders.insert(folderlist[i].toMap().value("FolderName").toString(),jsonToFolder(folderlist[i].toMap()));
        }
        folder.insert("folders",folders);
    }
    return folder;
}

void IVLEFetcher::gotReply(QNetworkReply *reply){
    qDebug()<<"redirect"<<reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    bool toDelete = true;
    QString p = reply->url().path();
    if (reply->error() == QNetworkReply::NoError)
    {
        if(p == QString("/api/Lapi.svc/Validate")){
            QVariantMap map = QJsonDocument::fromJson(reply->readAll()).toVariant().toMap();
            QString newToken = map.value("Token").toString();
            if(newToken != token){
                this->token = newToken;
                emit tokenUpdated(newToken);
            }
            fetchUserInfo();
        }else if(p == QString("/api/Lapi.svc/UserName_Get")){
            QByteArray re = reply->readAll();
            qDebug()<<re;
            _username = QJsonDocument::fromJson(re).toVariant().toString();
            qDebug()<<_username;
            emit statusUpdate(gottenUserInfo);
            fetchModules();
        }else if(p == QString("/api/Lapi.svc/Modules")){
            courses.clear();
            QVariantList courseList = QJsonDocument::fromJson(reply->readAll()).toVariant().toMap().value("Results").toList();
            for(int i = 0; i < courseList.count(); i++){
                QVariantMap map = courseList[i].toMap();
                if(map.value("isActive") == QString("Y")){
                    QVariantMap course;
                    course.insert("name",map.value("CourseCode").toString().replace('/',"-"));
                    courses.insert(map.value("ID").toString(), course);
                }
            }
            fetchWorkbins();
        }else if(p == QString("/api/Lapi.svc/Workbins")){
            //if more than 1 workbin, just fetch the first one
            //TODO: multiple workbin support.
            QVariantList resultsList = QJsonDocument::fromJson(reply->readAll()).toVariant().toMap().value("Results").toList();
            if(resultsList.count() > 0){
                //otherwise, no workbin
                QVariantMap top = jsonToFolder(resultsList[0].toMap());
                QString key = courses.keys()[currentWebBinFetching];
                QVariantMap tmp = courses.value(key).toMap();
                courses.remove(key);
                tmp.insert("filesystem",top);
                courses.insert(key,tmp);
            }
            fetchWorkBin();
        }else if(p == QString("/api/downloadfile.ashx")){
            updateDownload();
            emit fileDownloaded(toDownload.value(QUrlQuery(reply->url()).queryItemValue("ID")).toString());
            toDelete = false;
        }else{
            qDebug()<<p;
            //qDebug()<< QtJson::Json::parse(QString(reply->readAll()));
        }
    }
    else
    {
        qDebug()<<reply->errorString();
        // handle errors here
        emit statusUpdate(networkError);
    }
    if(toDelete){
        reply->deleteLater();
    }
}

QString IVLEFetcher::username(){
    return _username;
}

void IVLEFetcher::setDirectory(const QString &dir){
    if(QDir(dir) != this->path){
        delete session;
        session = new QObject(this);
        this->path = QDir(dir);
        this->start();
    }
}

void IVLEFetcher::exploreFolder(QDir& path, const QVariantMap& map){
    QVariantMap::Iterator it;
    QVariantMap files = map.value("files").toMap(), folders = map.value("folders").toMap(), file;
    for(it = files.begin(); it != files.end(); it++){
        file = it.value().toMap();
        if(!path.exists(file.value("name").toString())){
            toDownload.insert(it.key(),path.absoluteFilePath(file.value("name").toString()));
        }
    }
    for(it = folders.begin(); it != folders.end(); it++){
        if(!path.exists(it.key())){
            path.mkdir(it.key());
        }
        path.cd(it.key());
        exploreFolder(path,it.value().toMap());
    }
    path.cdUp();
}

void IVLEFetcher::buildDirectoriesAndDownloadList(){
    QVariantMap::Iterator it;
    QString name;
    QVariantMap filesystem, course;
    toDownload.clear();
    for(it = courses.begin(); it != courses.end(); it++){
        course = it.value().toMap();
        name = course.value("name").toString();
        filesystem = course.value("filesystem").toMap();
        if(!path.exists(name)){
            if(!path.mkpath(name)){
                qWarning("Failed to create directory!");
            }
        }
        path.cd(name);
        exploreFolder(path, filesystem);
    }
    emit statusUpdate(gottenWebbinInfo);
    download();
}

void IVLEFetcher::fetchWorkbins(){
    currentWebBinFetching = -1;
    fetchWorkBin();
}

void IVLEFetcher::fetchWorkBin(){
    currentWebBinFetching++;
    if(currentWebBinFetching < courses.count()){
        manager->get(QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/Lapi.svc/Workbins?APIKey=%1&AuthToken=%2&output=json&CourseID=%3&Duration=0").arg(APIKEY).arg(token).arg(courses.keys()[currentWebBinFetching]))));
    }else{
        buildDirectoriesAndDownloadList();
    }
}
void IVLEFetcher::validate(){
    qDebug()<<APIKEY;
    manager->get(QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/Lapi.svc/Validate?APIKey=%1&Token=%2&output=json").arg(APIKEY).arg(token))));
}

void IVLEFetcher::fetchUserInfo(){
    manager->get(QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/Lapi.svc/UserName_Get?APIKey=%1&Token=%2").arg(APIKEY).arg(token))));
}

void IVLEFetcher::fetchModules(){
    timer->stop();
    emit statusUpdate(gettingWebbinInfo);
    manager->get(QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/Lapi.svc/Modules?APIKey=%1&AuthToken=%2&Duration=0&IncludeAllInfo=false&output=json").arg(APIKEY).arg(token))));
}

void IVLEFetcher::download(){
    emit statusUpdate(downloading);
    numOfFiles = 0;
    for(QVariantMap::Iterator it = toDownload.begin(); it != toDownload.end(); it++){
        QNetworkReply *re = manager->get(QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/downloadfile.ashx?APIKey=%1&AuthToken=%2&ID=%3&target=workbin").arg(APIKEY).arg(token).arg(it.key()))));
        re->setParent(session);
        Downloader* dl = new Downloader(it.value().toString(),re,session);
        numOfFiles++;
    }
    updateDownload();
    //qDebug()<<toDownload.value(toDownload.keys()[0]);
}

void IVLEFetcher::updateDownload(){
    if(numOfFiles == 0){
        emit statusUpdate(complete);
        timer->start(300000);
    }else{
        emit statusUpdate(remainingChange);
    }
    numOfFiles--;
}

int IVLEFetcher::remainingFiles(){
    return numOfFiles;
}

void IVLEFetcher::setToken(const QString &t){
    if(token != t){
        delete session;
        session = new QObject(this);
        this->token = t;
        this->start();
    }
}

void IVLEFetcher::setMaxFileSize(double s){
    if(this->maxFileSize != s){
        delete session;
        session = new QObject(this);
        this->maxFileSize = s;
        this->start();
    }
}
