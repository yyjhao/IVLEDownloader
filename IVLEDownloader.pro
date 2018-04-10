#-------------------------------------------------
#
# Project created by QtCreator 2012-08-24T21:28:43
#
#-------------------------------------------------

QT       += core gui\
            widgets\
            webkitwidgets\
            network
CONFIG +=c++11
macx{
    QMAKE_CXXFLAGS = -mmacosx-version-min=10.7 -std=gnu++0x -stdlib=libc++
}

TARGET = IVLEDownloader
TEMPLATE = app


DEFINES += APIKEY=\\\"dy1R6cs39qzjQamrIzX6U\\\" \
            MAXRECENT=5

SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp \
    ivlefetcher.cpp \
    settings.cpp \
    downloader.cpp \
    recentfileaction.cpp \
    announcementsmenu.cpp \
    announcementaction.cpp \
    advanceddialog.cpp \
    lapi.cpp \
    promise.cpp \
    externalpageparser.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    ivlefetcher.h \
    settings.h \
    downloader.h \
    recentfileaction.h \
    announcementsmenu.h \
    announcementaction.h \
    advanceddialog.h \
    lapi.h \
    promise.h \
    externalpageparser.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    advanceddialog.ui

RESOURCES += \
    res.qrc
