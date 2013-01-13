#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose,false);


    QDir d = QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0];
    if(!d.exists()){
        QDir().mkpath(d.path());
    }
    logger = new QFile(d.filePath("download.log"));
    if(!logger->exists()){
        logger->open(QIODevice::WriteOnly);
        logger->close();
    }else{
        logger->open(QIODevice::ReadOnly);
        QString logs = logger->readAll();
        ui->plainTextEdit->setPlainText(logs);
        logger->close();
    }

    ivlefetcher = NULL;

    needStart = false;

    settingsDialog = new SettingsDialog(this);

    connect(ui->settingsBut, SIGNAL(clicked()), settingsDialog, SLOT(show()));

    settings = new Settings(this);
    settingsDialog->notifyCheck()->setChecked(settings->notify());
    settingsDialog->setMaxFileValue(int(settings->maxFileSize() / 1024 / 1024));

    connect(settingsDialog->notifyCheck(),SIGNAL(clicked(bool)),settings,SLOT(setNotify(bool)));
    connect(settingsDialog, SIGNAL(gottenToken(QString)), this, SLOT(processToken(QString)));
    connect(settingsDialog,SIGNAL(updateDirectory(QString)),settings,SLOT(setDirectory(QString)));
    connect(settingsDialog,SIGNAL(updateDirectory(QString)),this,SLOT(updateDirectory(QString)));
    connect(settingsDialog,SIGNAL(closedWithMaxFileSize(int)),this,SLOT(processMaxFileSize(int)));

    trayMenu = new QMenu(this);

    QAction* open = new QAction("Open",this);
    connect(open,SIGNAL(triggered()),this,SLOT(show()));
    trayMenu->addAction(open);

#ifdef Q_WS_MAC
    QAction* setting = new QAction(QString::fromUtf8("Preferences…"),this);
#else
    QAction* setting = new QAction("Settings",this);
#endif
    setting->setMenuRole(QAction::NoRole);
    connect(setting,SIGNAL(triggered()),settingsDialog,SLOT(show()));
    trayMenu->addAction(setting);

    trayMenu->addSeparator();

    updateAction = new QAction("Update Now",this);
    connect(updateAction,SIGNAL(triggered()),this,SLOT(updateFiles()));
    trayMenu->addAction(updateAction);
    updateAction->setDisabled(true);

    statusAction = new QAction("Please log in",this);
    statusAction->setDisabled(true);
    trayMenu->addAction(statusAction);

    recent = new QMenu("Recently downloaded files",this);
    int l = settings->recents().length();
    for(int i = 0; i < l; i++){
        recents[i] = new recentFileAction(this);
        recents[i]->setFile(settings->recents()[i]);
        recent->addAction(recents[i]);
    }
    if(!l){
        recent->setDisabled(true);
    }
    trayMenu->addMenu(recent);

    trayMenu->addSeparator();

    QAction* quit = new QAction("Quit",this);
    quit->setMenuRole(QAction::NoRole);
    connect(quit,SIGNAL(triggered()),qApp,SLOT(quit()));
    trayMenu->addAction(quit);

    normalIcon = QIcon(":/icons/icon.png");
    attnIcon = QIcon(":/icons/icon_attn.png");
    icon = new QSystemTrayIcon(normalIcon, this);
    icon->setContextMenu(trayMenu);
    icon->show();
    connect(trayMenu, SIGNAL(aboutToShow()), this, SLOT(menuToShow()));
    connect(icon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    if(settings->token().isEmpty() || settings->directory().isEmpty()){
        settingsDialog->show();
    }else{
        settingsDialog->setDlText(QString("Download to: %1").arg(settings->directory()));
        processToken(settings->token());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason r){
    icon->setIcon(normalIcon);
}

void MainWindow::menuToShow(){
    icon->setIcon(normalIcon);
}

void MainWindow::processMaxFileSize(int s){
    //s is in mb
    double size = (double)s * 1024 * 1024;
    settings->setMaxFileSize(size);
    ivlefetcher->setMaxFileSize(size);
}

void MainWindow::updateFiles(){
    if(ivlefetcher != NULL){
        if(needStart){
            ivlefetcher->start();
        }else{
            ivlefetcher->fetchModules();
        }
        updateAction->setDisabled(true);
    }
}

void MainWindow::logDownload(const QString &filename){
    qDebug()<<filename;
    logger->open(QIODevice::Append);
    QString toAdd = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm") + "\t" + filename + "\n";
    logger->write(toAdd.toStdString().c_str());
    logger->close();
    ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText() + toAdd);
    ui->plainTextEdit->scroll(0,ui->plainTextEdit->height());
    if(settings->notify()){
        icon->setIcon(attnIcon);
    }
}

void MainWindow::processToken(const QString& token){
    settings->setToken(token);
    if(!settings->directory().isEmpty()){
        if(ivlefetcher == NULL){
            createFetcher();
        }else{
            ivlefetcher->setToken(token);
        }
    }else{
        settingsDialog->setLabelText("Please set the download directory below.");
    }
}

void MainWindow::updateRecent(const QString &filename){
    settings->addRecentFile(filename);
    recent->setEnabled(true);
    const QStringList& _recent = settings->recents();
    int l = _recent.length() - 1;
    if(recent->actions().length() < _recent.length()){
        recents[l] = new recentFileAction(this);
        recent->addAction(recents[l]);
    }
    for(int i = 0; i <= l; i++){
        recents[i]->setFile(_recent[i]);
    }
}

void MainWindow::createFetcher(){
    ivlefetcher = new IVLEFetcher(settings->token(), settings->directory(), settings->maxFileSize(), this);
    connect(ivlefetcher,SIGNAL(statusUpdate(fetchingState)),this,SLOT(updateStatus(fetchingState)));
    connect(ivlefetcher,SIGNAL(tokenUpdated(QString)),this,SLOT(processToken(QString)));
    connect(ivlefetcher,SIGNAL(fileDownloaded(QString)),this,SLOT(logDownload(QString)));
    connect(ivlefetcher,SIGNAL(fileDownloaded(QString)),this,SLOT(updateRecent(QString)));
    ivlefetcher->start();
}

void MainWindow::updateStatus(fetchingState state){
    switch(state){
    case gettingUserInfo:
        settingsDialog->setLabelText("Fetching your infomation....");
        statusAction->setText("Verifying user infomation...");
        break;
    case gottenUserInfo:
        settingsDialog->setLabelText(QString("Currently logged in as %1").arg(ivlefetcher->username()));
        needStart = false;
        break;
    case gettingWebbinInfo:
        statusAction->setText("Retrieving the list of files...");
        break;
    case downloading:
        statusAction->setText("Getting ready to download...");
        break;
    case remainingChange:
        statusAction->setText(QString("%1 file(s) remaining...").arg(ivlefetcher->remainingFiles()));
        break;
    case complete:
        statusAction->setText("All files up to date");
        updateAction->setEnabled(true);
        break;
    case networkError:
        statusAction->setText("Failed to connect the network");
        updateAction->setEnabled(true);
        needStart = true;
        break;
    }
}

void MainWindow::updateDirectory(const QString &dir){
    settingsDialog->setDlText(QString("Download to: %1").arg(dir));
    if(!settings->token().isEmpty()){
        if(ivlefetcher == NULL){
            createFetcher();
        }else{
            ivlefetcher->setDirectory(dir);
            updateAction->setDisabled(true);
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui->plainTextEdit->setPlainText("");
    logger->remove();
    logger->open(QIODevice::WriteOnly);
    logger->close();
}
