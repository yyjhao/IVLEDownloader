#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    settingsPrivate = new QSettings("YYJHAO","IVLEDownloader");
    _token = settingsPrivate->value("token","").toString();
    _directory = settingsPrivate->value("directory","").toString();
    _notify = settingsPrivate->value("notify",true).toBool();
    _recents = settingsPrivate->value("recentFiles",QStringList()).toStringList();
}

bool Settings::notify(){
    return _notify;
}

void Settings::setNotify(bool val){
    settingsPrivate->setValue("notify",val);
    _notify = val;
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
