#ifndef ANNOUNCEMENTSMENU_H
#define ANNOUNCEMENTSMENU_H

#include <QMenu>
#include "announcementaction.h"

class AnnouncementsMenu : public QMenu
{
    Q_OBJECT
public:
    explicit AnnouncementsMenu(QWidget *parent = 0);
    void setAnnouncementItems(QVariantList);
    void empty();
signals:
    
public slots:

};

#endif // ANNOUNCEMENTSMENU_H
