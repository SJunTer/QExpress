#include "actdlg.h"
#include "ui_actdlg.h"
#include "../activate/activate.h"
#include <QMessageBox>
#include <string>
using namespace std;

ActDlg::ActDlg(Activate *a, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActDlg),
    act(a)
{
    ui->setupUi(this);
}

ActDlg::~ActDlg()
{
    delete ui;
}

void ActDlg::on_okBtn_clicked()
{
    if(ui->keyLine1->text().size()  != 4 || ui->keyLine2->text().size() != 4 ||
            ui->keyLine3->text().size() != 4 || ui->keyLine4->text().size() != 4)
    {
        QMessageBox::warning(this, "错误", "填写格式错误！", QMessageBox::Ok);
        return;
    }

    string input;
    input = ui->keyLine1->text().toStdString() + ui->keyLine2->text().toStdString()
            + ui->keyLine3->text().toStdString() + ui->keyLine4->text().toStdString();
    if(!act->Act_codeIsAgreed(input))
    {
        QMessageBox::warning(this, "错误", "密钥输入错误，无法激活！", QMessageBox::Ok);
        return;
    }
    else
    {
        act->WriteToFile();
        QMessageBox::information(this, "成功", "激活成功！", QMessageBox::Ok);
        this->accept();
    }
}
