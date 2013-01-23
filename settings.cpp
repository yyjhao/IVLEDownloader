#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    settingsPrivate = new QSettings("YYJHAO","IVLEDownloader");
    _token = settingsPrivate->value("token","").toString();
    _directory = settingsPrivate->value("directory","").toString();
    _notify = settingsPrivate->value("notify",true).toBool();
    _notifyAnnouncement = settingsPrivate->value("notifyAnnouncement",true).toBool();
    _recents = settingsPrivate->value("recentFiles",QStringList()).toStringList();
    _ignoreUploadable = settingsPrivate->value("ignoreUploadable", false).toBool();
    _lastAnnouncementTime = settingsPrivate->value("lastAnnouncementTime", QDateTime::currentDateTime()).toDateTime();
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


