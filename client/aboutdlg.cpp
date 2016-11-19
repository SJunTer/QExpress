#include "aboutdlg.h"
#include "ui_aboutdlg.h"

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDlg)
{
    ui->setupUi(this);
    setWindowTitle("关于QExpress");
}

AboutDlg::~AboutDlg()
{
    delete ui;
}

void AboutDlg::on_okBtn_clicked()
{
    emit closed();
    this->close();
}
