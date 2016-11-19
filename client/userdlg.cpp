#include "userdlg.h"
#include "ui_userdlg.h"

UserDlg::UserDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDlg)
{
    ui->setupUi(this);
    setWindowTitle("用户信息");
}

UserDlg::~UserDlg()
{
    delete ui;
}
