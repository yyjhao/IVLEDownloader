#ifndef ADVANCEDDIALOG_H
#define ADVANCEDDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QtNetwork>

namespace Ui {
class AdvancedDialog;
}

class AdvancedDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AdvancedDialog(QWidget *parent = 0);
    ~AdvancedDialog();

signals:
    void configSaved(const QVariantMap&);

public slots:
    void setConfigContent(const QString&);
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::AdvancedDialog *ui;
    QNetworkAccessManager *manager;
};

#endif // ADVANCEDDIALOG_H
