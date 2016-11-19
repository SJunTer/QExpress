#include "settingdlg.h"
#include "ui_settingdlg.h"
#include <QMessageBox>
#include <QDebug>

SettingDlg::SettingDlg(QString &s, int p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDlg),
    ip(s),
    port(p)
{
    ui->setupUi(this);
    ui->ipLineEdit->setText(ip);
    ui->portLineEdit->setText(QString::number(port));
    setWindowTitle("设置");
}

SettingDlg::~SettingDlg()
{
    delete ui;
}

void SettingDlg::on_okBtn_clicked()
{
    if(ui->ipLineEdit->text() == "" ||
            ui->portLineEdit->text() == "")
    {
        QMessageBox::warning(this, "提示", "请填写完整信息！", QMessageBox::Ok);
        return;
    }
    ip = ui->ipLineEdit->text();
    port = ui->portLineEdit->text().toInt();
    emit setInfo(ip, port);
    this->close();
}
