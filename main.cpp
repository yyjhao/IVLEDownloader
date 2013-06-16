#include <QtWidgets>
#include "mainwindow.h"
#include "lapi.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("YYJHAO");
    a.setApplicationName("IVLE Downloader");

//    MainWindow w;
    //w.show();
    Lapi* l = new Lapi("what");
    auto p = l->validate();
    QObject::connect(p, &Promise::done, [=](QVariant data){
        qDebug()<<data;
    });
    QObject::connect(p, &Promise::fail, [=](QVariant data){
        qDebug()<<data<<"err";
    });
    
    return a.exec();
}
