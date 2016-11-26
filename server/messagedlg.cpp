#include "messagedlg.h"
#include "ui_messagedlg.h"
#include <QMessageBox>
#include <QTableWidgetItem>


MessageDlg::MessageDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDlg)
{
    ui->setupUi(this);
    initTable();
}

MessageDlg::~MessageDlg()
{
    delete ui;
}

// 初始化表格
void MessageDlg::initTable()
{
//    ui->RegTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只可选择一个单元格
 //   ui->RegTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
}

// 接收注册请求
void MessageDlg::signUpReq(Account a)
{
    accounts.append(a);

    int rowCnt = ui->RegTable->rowCount();
    ui->RegTable->insertRow(rowCnt);

    QTableWidgetItem *item1 = new QTableWidgetItem;
    QTableWidgetItem *item2 = new QTableWidgetItem;
    QTableWidgetItem *item3 = new QTableWidgetItem;
    QTableWidgetItem *item4 = new QTableWidgetItem;

    item1->setText(a.username);
    item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
    item2->setText(a.name);
    item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
    item3->setText(a.phone);
    item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
    item4->setText(a.email);
    item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));

    ui->RegTable->setItem(rowCnt, 0, item1);
    ui->RegTable->setItem(rowCnt, 1, item2);
    ui->RegTable->setItem(rowCnt, 2, item3);
    ui->RegTable->setItem(rowCnt, 3, item4);
    ui->RegTable->setCurrentCell(rowCnt, 0);
}

// 客户端上传信息
void MessageDlg::getUpload(QString usr, int type, QString addr, QString detail)
{
    int rowCnt = ui->roadTable->rowCount();
    ui->roadTable->insertRow(rowCnt);

    QTableWidgetItem *item1 = new QTableWidgetItem;
    QTableWidgetItem *item2 = new QTableWidgetItem;
    QTableWidgetItem *item3 = new QTableWidgetItem;
    QTableWidgetItem *item4 = new QTableWidgetItem;

    item1->setText(usr);
    item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
    switch(type)
    {
    case ACCIDENT: item2->setText("事故"); break;
    case JAM: item2->setText("堵车"); break;
    case BLOCK: item2->setText("封路"); break;
    }
    item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
    item3->setText(addr);
    item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
    item4->setText(detail);
    item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));

    ui->roadTable->setItem(rowCnt, 0, item1);
    ui->roadTable->setItem(rowCnt, 1, item2);
    ui->roadTable->setItem(rowCnt, 2, item3);
    ui->roadTable->setItem(rowCnt, 3, item4);
    ui->roadTable->setCurrentCell(rowCnt, 0);
}

void MessageDlg::taskMsg(QString &s)
{
    ui->delvList->addItem(s);
}

// 同意注册
void MessageDlg::on_acceptBtn_clicked()
{
    int index = ui->RegTable->currentRow();
    if(index == -1)
        return;
    emit signUp(accounts[index]);
    ui->RegTable->removeRow(index);
    accounts.takeAt(index);
    QMessageBox::information(this, "提示", "帐号通过注册！", QMessageBox::Ok);
}

// 拒绝注册
void MessageDlg::on_rejectBtn_clicked()
{
    int index = ui->RegTable->currentRow();
    if(index == -1)
        return;
    if(QMessageBox::warning(this, "警告", "是否删除该用户请求？",
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
        return;
    ui->RegTable->removeRow(index);
    accounts.takeAt(index);
}
