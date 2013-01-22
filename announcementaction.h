#ifndef ANNOUNCEMENTSACTION_H
#define ANNOUNCEMENTSACTION_H

#include <QAction>
#include <QDesktopServices>
#include <QUrl>
#include <QDate>

class AnnouncementAction : public QAction
{
    Q_OBJECT
public:
    explicit AnnouncementAction(QVariantMap m, QObject *parent = 0);
    
signals:
    
public slots:

private slots:
    void viewAnnouncement();

private:
    QUrl url;
};

#endif // ANNOUNCEMENTSACTION_H
