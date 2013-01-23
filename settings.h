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

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
    QString token();
    QString directory();
    bool notify();
    const QStringList& recents();
    void addRecentFile(const QString&);
    double maxFileSize();
    bool ignoreUploadable();
    bool notifyAnnouncement();
    QDateTime lastAnnouncementTime();

signals:
    
public slots:
    void setToken(const QString&);
    void setDirectory(const QString&);
    void setNotify(bool val);
    void setMaxFileSize(double s);
    void setIgnoreUploadable(bool);
    void setNotifyAnnouncement(bool);
    void setLastAnnouncementTime(QDateTime);

private:
    QSettings* settingsPrivate;
    QString _token;
    QString _directory;
    QStringList _recents;
    bool _ignoreUploadable;
    bool _notify;
    bool _notifyAnnouncement;
    QDateTime _lastAnnouncementTime;
};

#endif // SETTINGS_H
