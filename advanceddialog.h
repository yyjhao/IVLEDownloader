#ifndef ADVANCEDDIALOG_H
#define ADVANCEDDIALOG_H

#include <QDialog>

namespace Ui {
class AdvancedDialog;
}

class AdvancedDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AdvancedDialog(QWidget *parent = 0);
    ~AdvancedDialog();
    
private:
    Ui::AdvancedDialog *ui;
};

#endif // ADVANCEDDIALOG_H
