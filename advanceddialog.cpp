#include "advanceddialog.h"
#include "ui_advanceddialog.h"

AdvancedDialog::AdvancedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedDialog)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
//    this->setWindowFlags((Qt::WindowFlags) (Qt::Window | Qt::WindowStaysOnTopHint | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint  & (~Qt::WindowFullscreenButtonHint)) );
}

AdvancedDialog::~AdvancedDialog()
{
    delete ui;
}

void AdvancedDialog::on_pushButton_clicked()
{
    if(QMessageBox::question(this, "Updating module info",
              "This will remove the current module info, are you sure?") == QMessageBox::Yes){
        ui->pushButton->setEnabled(false);
        ui->pushButton->setText("Checking...");
        auto r = manager->get(QNetworkRequest(QUrl("https://api.github.com/gists/5895119")));
        connect(r, &QNetworkReply::finished, [=](){
            if (r->error() == QNetworkReply::NoError){
                QByteArray re = r->readAll();
                auto data = QJsonDocument::fromJson(re).toVariant().toMap();
                ui->plainTextEdit->setPlainText(data["files"].toMap()["modules"].toMap()["content"].toString());
            }else{
                QMessageBox::critical(this, "Updating fail",
                    "Fail to obtain the updated info, please check your network connection");
            }
            r->deleteLater();
            ui->pushButton->setEnabled(true);
            ui->pushButton->setText("Update");
        });
    }
}

void AdvancedDialog::on_pushButton_2_clicked()
{
    auto json = QJsonDocument::fromJson(ui->plainTextEdit->toPlainText().toUtf8()).toVariant();
    if(json.isValid()){
        emit configSaved(json.toMap());
        QMessageBox::information(this, "Saved", "The json config is saved");
    }else{
        QMessageBox::critical(this, "Saving fail",
                    "The config is not valid");
    }
}

void AdvancedDialog::setConfigContent(const QString& data)
{
    ui->plainTextEdit->setPlainText(data);
}
