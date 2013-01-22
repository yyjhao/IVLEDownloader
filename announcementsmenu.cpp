#include "announcementsmenu.h"

AnnouncementsMenu::AnnouncementsMenu(QWidget *parent) :
    QMenu(parent)
{
    this->empty();
}

void AnnouncementsMenu::empty(){
    this->setTitle("No new announcements");
    this->setDisabled(true);
}

void AnnouncementsMenu::setAnnouncementItems(QVariantList l){
    this->clear();
    if(l.size() == 0){
        this->empty();
    }else{
        this->setTitle(QString("%1 new announcements").arg(l.size()));
        this->setDisabled(false);
        for(int i = 0; i < l.size(); i++){
            this->addAction(new AnnouncementAction(l[i].toMap(), this));
        }
//        this->addSeparator();
//        this->addAction(clearAction);
    }
}
