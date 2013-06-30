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
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::AdvancedDialog *ui;
    QNetworkAccessManager *manager;
};

#endif // ADVANCEDDIALOG_H
