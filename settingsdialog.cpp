#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    this->setWindowFlags((Qt::WindowFlags) (Qt::Window | Qt::WindowStaysOnTopHint | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint  & (~Qt::WindowFullscreenButtonHint)) );
    webviewDialog = new QDialog(this);
    webviewDialog->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
    webviewDialog->setAttribute(Qt::WA_QuitOnClose,false);
    webView = new QWebView(webviewDialog);
    webviewDialog->layout()->addWidget(webView);
    webviewDialog->layout()->setMargin(0);
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(onWebviewLoaded()));

    adialog = new AdvancedDialog(this);
    connect(adialog, &AdvancedDialog::configSaved, this, &SettingsDialog::configSaved);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::onWebviewLoaded(){
    QUrl url = webView->url();
    if(url.host() == QString("ivle.nus.edu.sg")&&
            url.path() == QString("/api/login/login_result.ashx")&&
            QUrlQuery(url).queryItemValue("r") == QString("0")){
        qDebug()<<"OK!";
        webviewDialog->close();
        emit gottenToken(webView->page()->mainFrame()->toPlainText());
    }
}

void SettingsDialog::on_pushButton_clicked()
{
    webView->setUrl(QString("https://ivle.nus.edu.sg/api/login/?apikey=%1").arg(APIKEY));
    //webviewDialog->setWindowModality(Qt::ApplicationModal);
    //setting modality cause the cursor to disappear in textbox...
    webviewDialog->show();
}

void SettingsDialog::setLabelText(const QString &t){
    ui->label->setText(t);
}

void SettingsDialog::setDlText(const QString &t){
    ui->label_2->setText(t);
}

void SettingsDialog::on_pushButton_2_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,"Choose a download destination");
    if(!path.isEmpty()){
        ui->label_2->setText(QString("Download to: %1").arg(path));
        ui->verticalLayout->update();
        emit updateDirectory(path);
    }
}

void SettingsDialog::setDisplayedSettings(QVariantMap m)
{
    ui->spinBox->setValue( (int)(m["maxFileSize"].toDouble() / 1024 / 1024) );
    ui->notifyCheckBox->setCheckState( m["notify"].toBool() ? Qt::Checked : Qt::Unchecked );
    ui->igUpCheckBox->setCheckState( m["ignoreUploadable"].toBool() ? Qt::Checked : Qt::Unchecked );
    ui->notifyAnmCheckBox->setCheckState( m["notifyAnm"].toBool() ? Qt::Checked : Qt::Unchecked );
    adialog->setConfigContent(m["jsonConfig"].toString());
}

bool SettingsDialog::event(QEvent *e)
{
    if(e->type() == QEvent::WindowActivate){
        this->setWindowFlags(this->windowFlags() & (~Qt::WindowStaysOnTopHint));
        this->show();
    }
    return QDialog::event(e);
}

void SettingsDialog::closeEvent(QCloseEvent *e){
    QVariantMap m;
    m["maxFileSize"] = ui->spinBox->value();
    m["ignoreUploadable"] = ui->notifyCheckBox->checkState() == Qt::Checked;
    m["notify"] = ui->igUpCheckBox->checkState() == Qt::Checked;
    m["notifyAnm"] = ui->notifyAnmCheckBox->checkState() == Qt::Checked;
    emit closedWithSettings(m);
    QDialog::closeEvent(e);
}

void SettingsDialog::on_pushButton_3_clicked()
{
    adialog->show();
}
