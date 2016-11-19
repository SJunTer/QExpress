#include "regdlg.h"
#include "ui_regdlg.h"
#include <QMessageBox>

RegDlg::RegDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegDlg)
{
    ui->setupUi(this);
    ui->usrLineEdit->setFocus();
    // 设置控件切换顺序
    setTabOrder(ui->usrLineEdit, ui->pwdLineEdit);
    setTabOrder(ui->pwdLineEdit, ui->pwd2LienEdit);
    setTabOrder(ui->pwd2LienEdit, ui->nameLineEdit);
    setTabOrder(ui->nameLineEdit, ui->emailLineEdit);
    setTabOrder(ui->emailLineEdit, ui->phoneLineEdit);
    setTabOrder(ui->phoneLineEdit, ui->agreeBox);
    setTabOrder(ui->agreeBox, ui->regBtn);
    setTabOrder(ui->regBtn, ui->cancelBtn);
    setTabOrder(ui->cancelBtn, ui->usrLineEdit);
    setWindowTitle("注册");
}

RegDlg::~RegDlg()
{
    delete ui;
}


void RegDlg::on_regBtn_clicked()
{
    // 检查信息是否完整
    if((acc.username = ui->usrLineEdit->text()) == "" ||
            (acc.password = ui->pwdLineEdit->text()) == "" ||
            (acc.name = ui->nameLineEdit->text()) == "" ||
            (acc.email = ui->emailLineEdit->text()) == "" ||
            (acc.phone = ui->phoneLineEdit->text()) == "")
    {
        QMessageBox::warning(this, "提示", "请填写完整信息！", QMessageBox::Ok);
        return;
    }
    if(ui->pwdLineEdit->text() != ui->pwd2LienEdit->text())
    {
        QMessageBox::warning(this, "提示", "两次填写密码不一致！", QMessageBox::Ok);
        return;
    }
    if(emit reg(acc) == true)
    {
        QMessageBox::warning(this, "成功", "注册成功，请稍后登陆！", QMessageBox::Ok);
        this->close();
        return;
    }
    else
     {
        QMessageBox::warning(this, "错误", "注册失败，请检查服务器连接！", QMessageBox::Ok);
        return;
    }
}
