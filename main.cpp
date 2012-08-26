#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("YYJHAO");
    a.setApplicationName("IVLE Downloader");

    MainWindow w;
    //w.show();
    
    return a.exec();
}
