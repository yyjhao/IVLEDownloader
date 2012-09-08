/*
 * A wrapper around QSettings to store configurations
 * By Yao Yujian
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QStringList>

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

signals:
    
public slots:
    void setToken(const QString&);
    void setDirectory(const QString&);
    void setNotify(bool val);
    void setMaxFileSize(double s);

private:
    QSettings* settingsPrivate;
    QString _token;
    QString _directory;
    QStringList _recents;
    bool _notify;
};

#endif // SETTINGS_H
