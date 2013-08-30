#include "settings.h"

Settings::Settings(QDir baseDir, QObject *parent) :
    QObject(parent)
{
    settingsPrivate = new QSettings("YYJHAO","IVLEDownloader");
    _token = settingsPrivate->value("token","").toString();
    _directory = settingsPrivate->value("directory","").toString();
    _notify = settingsPrivate->value("notify",true).toBool();
    _notifyAnnouncement = settingsPrivate->value("notifyAnnouncement",true).toBool();
    _recents = settingsPrivate->value("recentFiles",QStringList()).toStringList();
    _ignoreUploadable = settingsPrivate->value("ignoreUploadable", true).toBool();
    _lastAnnouncementTime = settingsPrivate->value("lastAnnouncementTime", QDateTime::currentDateTime()).toDateTime();
    pagesInfoFile = new QFile(baseDir.filePath("pagesInfo.json"), this);
    if(!pagesInfoFile->exists()){
        pagesInfoFile->open(QIODevice::WriteOnly);
        pagesInfoFile->close();
    }else{
        pagesInfoFile->open(QIODevice::ReadOnly);
        auto jsond = QJsonDocument::fromJson(pagesInfoFile->readAll());
        _pagesInfo = convertJSONToConfig(jsond.toVariant().toMap());
        _pagesInfoJson = jsond.toJson();
        pagesInfoFile->close();
   }
}

ExternalPageParser::Config Settings::pagesInfo(){
    return _pagesInfo;
}

QString Settings::pagesInfoJson(){
    return _pagesInfoJson;
}

ExternalPageParser::Config Settings::convertJSONToConfig(const QVariantMap& m)
{
    ExternalPageParser::Config cm;
    for(auto it = m.constBegin(); it != m.constEnd(); ++it){
        auto mm = it.value().toMap();
        QMap<QString, QPair<QUrl, QString>> cmm;
        for(auto itt = mm.constBegin(); itt != mm.constEnd(); ++itt){
            auto mmm = itt.value().toMap();
            cmm[itt.key()] = qMakePair(QUrl(mmm["page"].toString()), mmm["selector"].toString());
        }
        cm[it.key()] = cmm;
    }
    return cm;
}

void Settings::setConfig(const QVariantMap& m){
    _pagesInfo = convertJSONToConfig(m);
    _pagesInfoJson = QJsonDocument::fromVariant(QVariant(m)).toJson();
    pagesInfoFile->open(QIODevice::WriteOnly);
    pagesInfoFile->write(_pagesInfoJson.toUtf8());
    pagesInfoFile->close();
}

bool Settings::ignoreUploadable(){
    return _ignoreUploadable;
}

void Settings::setIgnoreUploadable(bool i){
    settingsPrivate->setValue("ignoreUploadable", i);
    _ignoreUploadable = i;
}

double Settings::maxFileSize(){
    return settingsPrivate->value("maxFileSize",50).toDouble();
}

void Settings::setMaxFileSize(double s){
    settingsPrivate->setValue("maxFileSize",s);
}

bool Settings::notify(){
    return _notify;
}

void Settings::setNotify(bool val){
    settingsPrivate->setValue("notify",val);
    _notify = val;
}

bool Settings::notifyAnnouncement(){
    return _notifyAnnouncement;
}

void Settings::setNotifyAnnouncement(bool val){
    settingsPrivate->setValue("notifyAnnouncement",val);
    _notifyAnnouncement = val;
}

QString Settings::token(){
    return _token;
}

void Settings::setToken(const QString &val){
    settingsPrivate->setValue("token",val);
    _token = val;
}


QString Settings::directory(){
    return _directory;
}

void Settings::setDirectory(const QString &val){
    settingsPrivate->setValue("directory",val);
    _directory = val;
}

const QStringList& Settings::recents(){
    return _recents;
}

void Settings::addRecentFile(const QString &f){
    if(_recents.count() >= MAXRECENT){
        _recents.pop_back();
    }
    _recents.push_front(f);
    settingsPrivate->setValue("recentFiles",_recents);
}

QDateTime Settings::lastAnnouncementTime(){
    return _lastAnnouncementTime;
}

void Settings::setLastAnnouncementTime(QDateTime d){
    _lastAnnouncementTime = d;
    settingsPrivate->setValue("lastAnnouncementTime", d);
}


