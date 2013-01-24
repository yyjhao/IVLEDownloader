/*
 * It has almost come to the point where I should refactor this
 * Notice the word "almost"
 */

#include "ivlefetcher.h"
#include <QDebug>

IVLEFetcher::IVLEFetcher(QString token, QVariantMap extras, QString dir, double maxfilesize, QObject *parent) :
    QObject(parent)
{
    this->token = token;
    this->path = QDir(dir);
    setExtraDownloads(extras);
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

void IVLEFetcher::setExtraDownloads(QVariantMap& m){
    extrasInfo.clear();
    // ignore any other 'folders' if . is present
    for(QVariantMap::iterator it = m.begin(); it != m.end(); it++){
        QString name = it.key();
        QVariantMap mm = it.value().toMap();
        QMap<QString, QString> item;
        if(mm["."].toMap().isEmpty()){
            for(QVariantMap::iterator itt = mm.begin(); itt != mm.end(); itt++){
                item["name"] = name;
                item["folder"] = itt.key();
                item["exec"] = itt.value().toMap()["exec"].toString();
                extrasInfo[itt.value().toMap()["page"].toString()] = item;
            }
        }else{
            QVariantMap info = mm["."].toMap();
            item["name"] = name;
            item["folder"] = ".";
            item["exec"] = info["exec"].toString();
            extrasInfo[info["page"].toString()] = item;
        }
    }
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
            QVariantMap f = folderlist[i].toMap();
            if(!(f.value("AllowUpload").toBool() && ignoreUploadable)){
                folders.insert(f.value("FolderName").toString(),jsonToFolder(f));
            }
        }
        folder.insert("folders",folders);
    }
    return folder;
}

void IVLEFetcher::setIgnoreUploadable(bool i){
    ignoreUploadable = i;
}

void IVLEFetcher::gotReply(QNetworkReply *reply){
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
            _username = QJsonDocument::fromJson(re).toVariant().toString();
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
        }else if(p == QString("/api/Lapi.svc/Announcements_Unread")){
            processAnnouncements(QJsonDocument::fromJson(reply->readAll()).toVariant().toMap().value("Results").toList());
        }else if(extrasInfo.contains(reply->url().toString())){
            qDebug()<<extrasInfo[reply->url().toString()]<<reply->url();
        }else{
            qDebug()<<p<<reply->readAll();
        }
    }
    else
    {
        // handle errors here
        emit statusUpdate(networkError);
    }
    if(toDelete){
        reply->deleteLater();
    }
}

void IVLEFetcher::processAnnouncements(QVariantList l){
    QVariantList out;
    QVariantMap cur, curOut;
    for(int i = 0; i < l.size(); i++){
        cur = l[i].toMap();
        QVariantList an = cur["Announcements"].toList();
        if(an.size() == 0)continue;
        QDateTime date = an[0].toMap()["CreatedDate_js"].toDateTime();
        for(int j = 1; j < an.size(); j++){
            QDateTime d = an[i].toMap()["CreatedDate_js"].toDateTime();
            if(d > date){
                date = d;
            }
        }
        curOut = QVariantMap();
        curOut["latest_date"] = date;
        curOut["course"] = cur["CourseCode"].toString();
        curOut["url"] = QString("https://ivle.nus.edu.sg/announcement/popup_list.aspx?CourseID=%1").arg(cur["ID"].toString());
        out.push_back(curOut);
    }
    emit(gotUnreadAnnouncements(out));
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

QVariantMap IVLEFetcher::mergeFiles(const QVariantMap &f1, const QVariantMap &f2){
    if(f1.empty())return f2;
    else if(f2.empty())return f1;
    return f1;
}

QVariantMap IVLEFetcher::mergeFileSystems(const QVariantMap &f1, const QVariantMap &f2){
    if(f1.empty())return QVariantMap(f2);
    else if(f2.empty())return QVariantMap(f1);
    QVariantMap r;
    r["files"] = mergeFiles(f1["files"].toMap(), f2["files"].toMap());
    QVariantMap fd1 = f1["folders"].toMap(), fd2 = f2["folders"].toMap(), rfd;
    QStringList k1 = f1.keys(), k2 = f2.keys();
    int i1 = 0, i2 = 0, s1 = k1.size(), s2 = k2.size();
    while(i1 < s1 && i2 < s2){
        if(k1[i1] < k2[i2]){
            rfd[k1[i1]] = fd1[k1[i1]];
            i1++;
        }else if(k2[i2] < k1[i1]){
            rfd[k2[i2]] = fd2[k2[i2]];
        }else{
            rfd[k1[i1]] = mergeFileSystems(fd1[k1[i1]].toMap(), fd2[k2[i2]].toMap());
        }
    }
    r["folders"] = rfd;
    return r;
}

QVariantMap IVLEFetcher::cleanFileSystem(const QVariantMap& filesystem){
    QVariantMap folders = filesystem["folders"].toMap();
    QStringList keys = folders.keys();
    QVariantMap folder;
    for(int i = 0; i < keys.size(); i++){
        folder = cleanFileSystem(folders[keys[i]].toMap());
        if(folder.isEmpty()){
            folders.remove(keys[i]);
        }else{
            folders[keys[i]] = folder;
        }
    }
    QVariantMap result;
    if(!folders.isEmpty()){
        result["folders"] = folders;
    }
    if(!filesystem["files"].toMap().isEmpty()){
        result["files"] = filesystem["files"];
    }
    return result;
}

void IVLEFetcher::workbinReady(){
    isWorkbinReady = true;
    if(isExtraReady){
        buildDirectoriesAndDownloadList();
    }
}

void IVLEFetcher::extraReady(){
    isExtraReady = true;
    if(isWorkbinReady){
        buildDirectoriesAndDownloadList();
    }
}


void IVLEFetcher::buildDirectoriesAndDownloadList(){
    QVariantMap::Iterator it;
    QString name;
    QVariantMap filesystem, course;
    toDownload.clear();
    for(it = courses.begin(); it != courses.end(); it++){
        course = it.value().toMap();
        name = course.value("name").toString();
        filesystem = cleanFileSystem(mergeFileSystems(course.value("filesystem").toMap(), extras[name].toMap()));
        // module has no files, skip
        if(filesystem.isEmpty())continue;
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
        workbinReady();
    }
}

void IVLEFetcher::fetchAnnouncement(){
    manager->get(QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/Lapi.svc/Announcements_Unread?APIKey=%1&AuthToken=%2&TitleOnly=true&output=json").arg(APIKEY).arg(token))));
}

void IVLEFetcher::validate(){
    manager->get(QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/Lapi.svc/Validate?APIKey=%1&Token=%2&output=json").arg(APIKEY).arg(token))));
}

void IVLEFetcher::fetchUserInfo(){
    manager->get(QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/Lapi.svc/UserName_Get?APIKey=%1&Token=%2").arg(APIKEY).arg(token))));
}

void IVLEFetcher::fetchModules(){
    timer->stop();
    emit statusUpdate(gettingWebbinInfo);
    fetchAnnouncement();
    fetchExtras();
    manager->get(QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/Lapi.svc/Modules?APIKey=%1&AuthToken=%2&Duration=0&IncludeAllInfo=false&output=json").arg(APIKEY).arg(token))));
}

void IVLEFetcher::fetchExtras(){
    extras.clear();
    QStringList k = extrasInfo.keys();
    extrasToFetch = k.size();
    for(int i = 0; i < extrasToFetch; i++){
        manager->get(QNetworkRequest(QUrl(k[i])));
    }
    if(extrasToFetch == 0){
        extraReady();
    }
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
