#include "recentfileaction.h"

recentFileAction::recentFileAction(QObject *parent) :
    QAction(parent)
{
    connect(this,SIGNAL(triggered()),this,SLOT(openFile()));
}


void recentFileAction::setFile(const QString& filePath){
    this->filePath = QUrl(QString("file://")+filePath);
    QString text = filePath;
    int last = 0;
    for(int i = 0; i < 3; i++){
        last = text.lastIndexOf('/',last - 1);
    }
    text.remove(0,last + 1);
    qDebug()<<text<<filePath;
    this->setText(text);
}

void recentFileAction::openFile(){
    QDesktopServices::openUrl(filePath);
}
