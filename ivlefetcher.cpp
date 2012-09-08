#include "ivlefetcher.h"
#include <QDebug>

IVLEFetcher::IVLEFetcher(QString token, QString dir, QObject *parent) :
    QObject(parent)
{
    this->token = token;
     this->path = QDir(dir);
    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(gotReply(QNetworkReply*)));
    session = new QObject(this);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(fetchModules()));
}

void IVLEFetcher::start(){
    emit statusUpdate(gettingUserInfo);
    validate();
}

void IVLEFetcher::gotReply(QNetworkReply *reply){
    qDebug()<<"redirect"<<reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    bool toDelete = true;
    QString p = reply->url().path();
    if (reply->error() == QNetworkReply::NoError)
    {
        if(p == QString("/api/Lapi.svc/Validate")){
            QVariantMap map = QtJson::Json::parse(QString(reply->readAll())).toMap();
            QString newToken = map.value("Token").toString();
            if(newToken != token){
                this->token = newToken;
                emit tokenUpdated(newToken);
            }
            fetchUserInfo();
        }else if(p == QString("/api/Lapi.svc/UserName_Get")){
            QString re = QString(reply->readAll());
            qDebug()<<re;
            _username = QtJson::Json::parse(re).toString();
            qDebug()<<_username;
            emit statusUpdate(gottenUserInfo);
            fetchModules();
        }else if(p == QString("/api/Lapi.svc/Modules")){
            courses.clear();
            QVariantList courseList = QtJson::Json::parse(QString(reply->readAll())).toMap().value("Results").toList();
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
            QVariantList resultsList = QtJson::Json::parse(QString(reply->readAll())).toMap().value("Results").toList();
            if(resultsList.count() > 0){
                //otherwise, no workbin
                QVariantList folderlist = resultsList[0].toMap().value("Folders").toList();
                QVariantMap folders;
                for(int i = 0; i < folderlist.count(); i++){
                    QVariantMap folder, files;
                    QVariantList filelist = folderlist[i].toMap().value("Files").toList();
                    for(int j = 0; j < filelist.count(); j++){
                        QVariantMap file, fileJS = filelist[j].toMap();
                        file.insert("name",fileJS.value("FileName"));
                        file.insert("uploadTime",fileJS.value("UploadTime_js").toDate());
                        folder.insert(fileJS.value("ID").toString(),file);
                    }
                    folders.insert(folderlist[i].toMap().value("FolderName").toString(),folder);
                }
                QString key = courses.keys()[currentWebBinFetching];
                QVariantMap tmp = courses.value(key).toMap();
                courses.remove(key);
                tmp.insert("folders",folders);
                courses.insert(key,tmp);
            }
            fetchWorkBin();
        }else if(p == QString("/api/downloadfile.ashx")){
            updateDownload();
            emit fileDownloaded(toDownload.value(reply->url().queryItemValue("ID")).toString());
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

void IVLEFetcher::buildDirectoriesAndDownloadList(){
    QVariantMap::Iterator it,itt, ittt;
    QString name;
    QVariantMap folders, course,files,file;
    toDownload.clear();
    for(it = courses.begin(); it != courses.end(); it++){
        course = it.value().toMap();
        name = course.value("name").toString();
        folders = course.value("folders").toMap();
        if(!path.exists(name)){
            if(!path.mkpath(name)){
                qWarning("Failed to create directory!");
            }
        }
        path.cd(name);
        for(itt = folders.begin(); itt != folders.end(); itt++){
            files = itt.value().toMap();
            if(files.count() == 0){
                qDebug()<<itt.key();
            }else{
                if(!path.exists(itt.key())){
                    path.mkdir(itt.key());
                }
                path.cd(itt.key());
                for(ittt = files.begin(); ittt != files.end(); ittt++){
                    file = ittt.value().toMap();
                    //TODO: update files if the file found is older than the one in webbin
                    if(!path.exists(file.value("name").toString())){
                        toDownload.insert(ittt.key(),path.absoluteFilePath(file.value("name").toString()));
                    }
                }
                path.cdUp();
            }
        }
        path.cdUp();
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
