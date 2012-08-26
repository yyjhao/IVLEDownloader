/*
 * Just the settings interface
 * By Yao Yujian
 */

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtWebKit>
#include <QFileDialog>
#include <QCheckBox>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    QCheckBox* notifyCheck();

public slots:
    void setLabelText(const QString&);
    void setDlText(const QString&);

    
private slots:
    void on_pushButton_clicked();
    void onWebviewLoaded();

    void on_pushButton_2_clicked();

signals:
    void gottenToken(QString);
    void updateDirectory(QString);

private:
    Ui::SettingsDialog *ui;
    QWebView* webView;
    QDialog* webviewDialog;
};

#endif // SETTINGSDIALOG_H
