#-------------------------------------------------
#
# Project created by QtCreator 2012-08-24T21:28:43
#
#-------------------------------------------------

QT       += core gui\
            widgets\
            webkitwidgets\
            network

TARGET = IVLEDownloader
TEMPLATE = app


DEFINES += APIKEY=\\\"k0z3B5Ng9rhy3MKVAKsGG\\\" \
            MAXRECENT=5

SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    ivlefetcher.cpp \
    settings.cpp \
    downloader.cpp \
    recentfileaction.cpp \
    announcementsmenu.cpp \
    announcementaction.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    ivlefetcher.h \
    settings.h \
    downloader.h \
    recentfileaction.h \
    announcementsmenu.h \
    announcementaction.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    res.qrc
