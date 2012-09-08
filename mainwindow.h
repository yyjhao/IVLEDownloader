/*
 * Just the main interface
 * By Yao Yujian
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"
#include "settings.h"
#include "ivlefetcher.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QDesktopServices>
#include <QDateTime>
#include "recentfileaction.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void processToken(const QString&);
    void updateStatus(fetchingState);
    void updateDirectory(const QString&);
    void logDownload(const QString& filename);
    void updateFiles();
    void iconActivated(QSystemTrayIcon::ActivationReason);
    void createFetcher();
    void updateRecent(const QString& filename);
    void processMaxFileSize(int);
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    SettingsDialog* settingsDialog;
    Settings* settings;
    IVLEFetcher* ivlefetcher;
    QSystemTrayIcon* icon;
    QMenu* trayMenu;
    QAction* statusAction;
    QAction* updateAction;
    QFile* logger;
    QIcon normalIcon;
    QIcon attnIcon;
    bool needStart;
    QMenu* recent;
    recentFileAction* recents[MAXRECENT];
};

#endif // MAINWINDOW_H
