/*
 * It has almost come to the point where I should refactor this
 * Notice the word "almost"
 */

#include "ivlefetcher.h"
#include <QDebug>

IVLEFetcher::IVLEFetcher(QString token, ExternalPageParser* parser, QString dir, double maxfilesize, QObject *parent) :
    QObject(parent)
{
    this->token = token;
    this->path = QDir(dir);
    this->parser = parser;
    session = new QObject(this);
    this->maxFileSize = maxfilesize;
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(start()));
    api = new Lapi(token, this);
}

void IVLEFetcher::start(){
    timer->stop();
    api->validate()->then([=](const QVariant& data){
        auto t = data.toString();
        if(t != token){
            token = t;
            emit tokenUpdated(token);
        }
        api->fetchAnnouncement()->then([=](const QVariant& data){
            processAnnouncements(data.toMap().value("Results").toList());
        });
    })->pipe([=](const QVariant&){
        emit statusUpdate(gettingUserInfo);
        return api->fetchUserInfo()->then([=](const QVariant& data){
            _username = data.toString();
            emit statusUpdate(gottenUserInfo);
        });
    })->pipe([=](const QVariant&){
        return api->fetchModules();
    })->pipe([=](const QVariant& data){
        emit statusUpdate(gettingWebbinInfo);
        QVariantList modules = data.toMap().value("Results").toList();
        QList<Promise*> ps;
        QStringList moduleNames;
        for(auto module : modules){
            QVariantMap map = module.toMap();
            if(map.value("isActive") == QString("Y")){
                QVariantMap course;
                QString id = map.value("ID").toString();
                QString name = map.value("CourseCode").toString().replace('/',"-");
                moduleNames.push_back(name);
                QVariantMap tmpc;
                tmpc["name"] = name;
                courses[id] = tmpc;
                ps.push_back(api->fetchWorkbin(id)->then([=](const QVariant& data){
                    qDebug()<<"ok"<<name;
                    QVariantList resultsList = data.toMap().value("Results").toList();
                    int c = 0;
                    for(auto workbin : resultsList){
                        QVariantMap course;
                        if(c){
                            course["name"] = QString("%1-%2").arg(name).arg(c + 1);
                        }else{
                            course["name"] = name;
                        }
                        course["filesystem"] = jsonToFolder(workbin.toMap());
                        courses[id] = course;
                        c++;
                    }
                }, [=](const QVariant& err){
                    qDebug()<<"err"<<name<<err;
                }));
            }
        }
        ps.push_back(parser->fetchFileInfo(moduleNames)->then([=](const QVariant& data){
            extras = data.toMap();
            qDebug()<<"extras"<<extras;
        }));
        return Promise::all(ps, this);            
    })->pipe([=](const QVariant&){
        emit statusUpdate(gottenWebbinInfo);
        buildDirectoriesAndDownloadList();
        emit statusUpdate(downloading);
        numOfFiles = 0;
        auto manager = api->getManager();
        QList<Promise*> ps;
        for(auto it = toDownload.begin(); it != toDownload.end(); it++){
            qDebug()<<"down"<<it.value();
            QNetworkReply *re = manager->get(QNetworkRequest(QUrl(it.key())));
            re->setParent(session);
            Downloader* dl = new Downloader(it.value().toString(),re,session);
            numOfFiles++;
            ps.push_back(dl->getPromise()->then([=](const QVariant& data){
                numOfFiles--;
                emit statusUpdate(remainingChange);
                emit fileDownloaded(data.toString());
                qDebug()<<data;
            }));
        }
        toDownload.clear();
        return Promise::all(ps, this->session);
    })->then([=](const QVariant& data){
        emit statusUpdate(complete);
        timer->start(3600000);
        qDebug()<<data;
    }, [=](const QVariant& error){
        if(error.toString() == "invalid token"){
            emit statusUpdate(invalidToken);
        } else {
            qDebug()<<"err"<<error;
            emit statusUpdate(networkError);
            timer->start(3600000);
        }
    });
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
            files.insert(api->getWorkBinDownloadUrl(fileJS.value("ID").toString()),file);
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

void IVLEFetcher::processAnnouncements(QVariantList l){
    QVariantList out;
    QVariantMap cur, curOut;
    for(int i = 0; i < l.size(); i++){
        cur = l[i].toMap();
        QVariantList an = cur["Announcements"].toList();
        if(an.size() == 0)continue;
        QDateTime date = an[0].toMap()["CreatedDate_js"].toDateTime();
        for(int j = 1; j < an.size(); j++){
            QDateTime d = an[j].toMap()["CreatedDate_js"].toDateTime();
            if(d > date){
                date = d;
            }
        }
        curOut = QVariantMap();
        curOut["latest_date"] = date;
        curOut["course"] = cur["CourseCode"].toString();
        curOut["url"] = QString("https://ivle.nus.edu.sg/v1/Announcement/default.aspx?CourseID=%1").arg(cur["ID"].toString());
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
    QVariantMap m;
    QVariantMap::const_iterator it;
    for(it = f1.begin(); it != f1.end(); it++){
        m[it.key()] = it.value();
    }
    for(it = f2.begin(); it != f2.end(); it++){
        m[it.key()] = it.value();
    }
    return m;
}

QVariantMap IVLEFetcher::mergeFileSystems(const QVariantMap &f1, const QVariantMap &f2){
    if(f1.empty())return f2;
    else if(f2.empty())return f1;
    QVariantMap r;
    r["files"] = mergeFiles(f1["files"].toMap(), f2["files"].toMap());
    QVariantMap fd1 = f1["folders"].toMap(), fd2 = f2["folders"].toMap(), rfd;
    QStringList k1 = fd1.keys(), k2 = fd2.keys();
    int i1 = 0, i2 = 0, s1 = k1.size(), s2 = k2.size();
    while(i1 < s1 && i2 < s2){
        if(k1[i1] < k2[i2]){
            rfd[k1[i1]] = fd1[k1[i1]];
            i1++;
        }else if(k2[i2] < k1[i1]){
            rfd[k2[i2]] = fd2[k2[i2]];
            i2++;
        }else{
            rfd[k1[i1]] = mergeFileSystems(fd1[k1[i1]].toMap(), fd2[k2[i2]].toMap());
            i1++;
            i2++;
        }
    }
    while(i1 < s1){
        rfd[k1[i1]] = fd1[k1[i1]];
        i1++;
    }
    while(i2 < s2){
        rfd[k2[i2]] = fd2[k2[i2]];
        i2++;
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

void IVLEFetcher::buildDirectoriesAndDownloadList(){
    for(auto c : courses){
        auto course = c.toMap();
        QString name = course["name"].toString();
        auto filesystem = cleanFileSystem(mergeFileSystems(course.value("filesystem").toMap(), extras[name].toMap()));
        // module has no files, skip
        if(name == QString("Rmodules")){
            qDebug()<<filesystem<<"eh";
        }
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
}

int IVLEFetcher::remainingFiles(){
    return numOfFiles;
}

void IVLEFetcher::setToken(const QString &t){
    if(token != t){
        delete session;
        session = new QObject(this);
        this->token = t;
        this->api->setToken(t);
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
