/*
 * A wrapper around QSettings to store configurations
 * Could perhaps be simplified
 * By Yao Yujian
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QDateTime>
#include <QJsonDocument>
#include <QDir>
#include <QFile>
#include <QVariant>
#include "externalpageparser.h"

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QDir baseDir, QObject *parent = 0);
    QString token();
    QString directory();
    bool notify();
    const QStringList& recents();
    void addRecentFile(const QString&);
    double maxFileSize();
    bool ignoreUploadable();
    bool notifyAnnouncement();
    QDateTime lastAnnouncementTime();
    ExternalPageParser::Config pagesInfo();
    QString pagesInfoJson();

    static ExternalPageParser::Config convertJSONToConfig(const QVariantMap&);

signals:
    
public slots:
    void setToken(const QString&);
    void setDirectory(const QString&);
    void setNotify(bool val);
    void setMaxFileSize(double s);
    void setIgnoreUploadable(bool);
    void setNotifyAnnouncement(bool);
    void setLastAnnouncementTime(QDateTime);
    void setConfig(const QVariantMap&);

private:
    QSettings* settingsPrivate;
    QString _token;
    QString _directory;
    QStringList _recents;
    bool _ignoreUploadable;
    bool _notify;
    bool _notifyAnnouncement;
    QDateTime _lastAnnouncementTime;
    ExternalPageParser::Config _pagesInfo;
    QFile* pagesInfoFile;
    QString _pagesInfoJson;
};

#endif // SETTINGS_H
