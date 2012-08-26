#-------------------------------------------------
#
# Project created by QtCreator 2012-08-24T21:28:43
#
#-------------------------------------------------

QT       += core gui\
            webkit\
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
    json.cpp \
    downloader.cpp \
    recentfileaction.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h \
    ivlefetcher.h \
    settings.h \
    json.h \
    downloader.h \
    recentfileaction.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    res.qrc
