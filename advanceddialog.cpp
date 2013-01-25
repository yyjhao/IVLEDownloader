#include "advanceddialog.h"
#include "ui_advanceddialog.h"

AdvancedDialog::AdvancedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdvancedDialog)
{
    ui->setupUi(this);
    this->setWindowFlags((Qt::WindowFlags) (Qt::Window | Qt::WindowStaysOnTopHint | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint  & (~Qt::WindowFullscreenButtonHint)) );
}

AdvancedDialog::~AdvancedDialog()
{
    delete ui;
}
