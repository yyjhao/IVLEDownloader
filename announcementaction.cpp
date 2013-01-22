#include "announcementaction.h"

AnnouncementAction::AnnouncementAction(QVariantMap m, QObject *parent) :
    QAction(parent)
{
    url = QUrl(m["url"].toString());
    setText(QString("%1 (%2)").arg(m["course"].toString()).arg(m["latest_date"].toDate().toString()));
    connect(this, SIGNAL(triggered()), this, SLOT(viewAnnouncement()));
}

void AnnouncementAction::viewAnnouncement(){
    QDesktopServices::openUrl(url);
}
