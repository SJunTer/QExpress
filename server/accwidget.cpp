#include "accwidget.h"
#include "nofocusdelegate.h"
#include "database/operatesql.h"
#include "database/operatedefine.h"
#include <string>
#include <vector>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QBoxLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
using namespace std;

AccWidget::AccWidget(Dbsql *d, QWidget *parent) :
    QWidget(parent),
    dbsql(d)
{
    accTable = new QTableWidget(this);
    addBtn = new QPushButton(this);
    delBtn = new QPushButton(this);
    closeBtn = new QPushButton(this);

    addBtn->setText("添加");
    addBtn->setEnabled(false);
    addBtn->setFixedSize(80,35);
    delBtn->setText("删除");
    delBtn->setFixedSize(80,35);
    closeBtn->setText("断开连接");
    closeBtn->setFixedSize(100,35);

    initTable();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(10);
    mainLayout->setMargin(20);
    mainLayout->addWidget(accTable);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(5);
    btnLayout->setMargin(0);
    btnLayout->addStretch();
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    connect(addBtn, SIGNAL(clicked(bool)), this, SLOT(addAccount()));
    connect(delBtn, SIGNAL(clicked(bool)), this, SLOT(delAccount()));
    connect(closeBtn, SIGNAL(clicked(bool)), this, SLOT(closeConn()));

}

AccWidget::~AccWidget()
{
}

//初始化表窗体样式
void AccWidget::initTable()
{
    accTable->setColumnCount(7);
    accTable->horizontalHeader()->setDefaultSectionSize(150);
    accTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header << tr("序号") << tr("用户名") << tr("姓名") << tr("电话") << tr("邮箱")
           << tr("当前状态") << tr("最后登陆时间");
    accTable->setHorizontalHeaderLabels(header);

    QFont font = accTable->horizontalHeader()->font();
    font.setBold(true);
    accTable->horizontalHeader()->setFont(font);

    accTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    accTable->verticalHeader()->setDefaultSectionSize(20);
    accTable->verticalHeader()->setVisible(false);
    accTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只可选择一个单元格
//    accTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    accTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    accTable->horizontalHeader()->setFixedHeight(30); //设置表头的高度
    accTable->setItemDelegate(new NoFocusDelegate(this));  // 消除选中虚框
}

// 初始化账户List
void AccWidget::readInfo()
{
    vector<string> accs;
    if(dbsql->AllSearch(TABLE_ACCOUNT, accs) != 0)
    {
        qDebug() << "ERROR on search";
        return;
    }
    for(unsigned i = 0; i < accs.size(); i+=ACCOUNT_COL_NUM)
    {
        Account a;
        a.id = stoi(accs[i]);
        a.username = QString::fromStdString(accs[i+1]);
        a.password = "";
        a.name = QString::fromStdString(accs[i+4]);
        a.phone = QString::fromStdString(accs[i+5]);
        a.email = QString::fromStdString(accs[i+6]);
        a.status = stoi(accs[i+7]);
//        a.status = OFFLINE;
        a.last_time = QString::fromStdString(accs[i+8]);
        accounts.push_back(a);

        int rowCnt = accTable->rowCount();
        accTable->insertRow(rowCnt);
        QTableWidgetItem *item1 = new QTableWidgetItem;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QTableWidgetItem *item4 = new QTableWidgetItem;
        QTableWidgetItem *item5 = new QTableWidgetItem;
        QTableWidgetItem *item6 = new QTableWidgetItem;
        QTableWidgetItem *item7 = new QTableWidgetItem;
        item1->setText(QString::number(a.id));
        item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
        item2->setText(a.username);
        item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
        item3->setText(a.name);
        item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
        item4->setText(a.phone);
        item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
        item5->setText(a.email);
        item5->setFlags(item5->flags() & (~Qt::ItemIsEditable));
        switch(a.status)
        {
        case OFFLINE: item6->setText("离线"); break;
        case ONLINE: item6->setText("在线"); break;
        case WORK: item6->setText("执行任务中"); break;
        }
        item6->setFlags(item6->flags() & (~Qt::ItemIsEditable));
        item7->setText(a.last_time);
        item7->setFlags(item7->flags() & (~Qt::ItemIsEditable));

        accTable->setItem(rowCnt, 0, item1);
        accTable->setItem(rowCnt, 1, item2);
        accTable->setItem(rowCnt, 2, item3);
        accTable->setItem(rowCnt, 3, item4);
        accTable->setItem(rowCnt, 4, item5);
        accTable->setItem(rowCnt, 5, item6);
        accTable->setItem(rowCnt, 6, item7);
        accTable->setCurrentCell(0, 0);
    }
    emit sendAccounts(&accounts);
}

void AccWidget::addAccount()
{
    // TODO
}

// 删除用户
void AccWidget::delAccount()
{
    int index = accTable->currentRow();
    if(index == -1)
    {
        QMessageBox::warning(this, tr("错误"), tr("未选中项！"));
        return;
    }
    else
    {
        if(QMessageBox::warning(this, "警告", "确定删除改用户吗？",
                                QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
            return;

        emit closeClient(accounts[index].id);
        if(dbsql->Delete(TABLE_ACCOUNT, ACCOUNT_ID, QString::number(accounts[index].id).toStdString()) != 0)
        {
            QMessageBox::warning(this, tr("错误"), tr("删除异常！"));
            return;
        }
        accTable->removeRow(index);
        accounts.takeAt(index);
        emit sendAccounts(&accounts);
        QMessageBox::information(this, tr("提示"), tr("删除成功！"));
    }
}

// 切断连接
void AccWidget::closeConn()
{
    qDebug() << "close client";

    int index = accTable->currentRow();
    if(index == -1)
    {
        QMessageBox::warning(this, tr("错误"), tr("未选中项！"));
        return;
    }

    if(accounts[index].status == OFFLINE)   // 如果用户未上线
    {
        QMessageBox::warning(this, tr("错误"), tr("该用户处于离线状态！"));
        return;
    }
    if(QMessageBox::warning(this, "警告","切断客户端连接可能丢失信息，是否继续？",
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
    {
        return;
    }
    accounts[index].status = OFFLINE;
    accTable->item(index, 5)->setText("离线");
    emit sendAccounts(&accounts);

    vector<string> acc;
    acc.push_back("");
    acc.push_back("");
    acc.push_back("");
    acc.push_back("");
    acc.push_back("");
    acc.push_back("");
    acc.push_back("");
    acc.push_back(QString::number(OFFLINE).toStdString());
    acc.push_back("");
    if(dbsql->Alter(TABLE_ACCOUNT, ACCOUNT_ID, QString::number(accounts[index].id).toStdString(), acc) != 0)
        qDebug() << "ERROR on alter";

    emit closeClient(accounts[index].id);

}

//用户登录
void AccWidget::signIn(int id)
{
    qDebug() << "sign in";
    for(int i = 0; i < accounts.size(); ++i)
    {
        if(accounts[i].id == id)
        {
            accounts[i].status = ONLINE;
            accounts[i].last_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            accTable->item(i, 5)->setText("在线");
            accTable->item(i, 6)->setText(accounts[i].last_time);
            emit sendAccounts(&accounts);

            vector<string> acc;
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back(QString::number(ONLINE).toStdString());
            acc.push_back(accounts[i].last_time.toStdString());
            if(dbsql->Alter(TABLE_ACCOUNT, ACCOUNT_ID, QString::number(id).toStdString(), acc) != 0)
                qDebug() << "ERROR on alter";
            break;
        }
    }
}

// 用户注册
void AccWidget::signUp(Account a)
{
    qDebug() << "sign up";
    if(accounts.empty())
        a.id = 0;
    else
        a.id = accounts.back().id+1;
    a.status = OFFLINE;
    //添加到数据库中
    vector<string> acc;
    acc.push_back(QString::number(a.id).toStdString());
    acc.push_back(a.username.toStdString());
    acc.push_back(a.password.toStdString());
    acc.push_back(a.name.toStdString());
    acc.push_back(a.phone.toStdString());
    acc.push_back(a.email.toStdString());
    acc.push_back(QString::number(OFFLINE).toStdString());
    acc.push_back("");
    if(dbsql->Insert(TABLE_ACCOUNT, acc) != 0)
    {
        qDebug() << "ERROR on insert";
        return;
    }
    accounts.push_back(a);
    emit sendAccounts(&accounts);

    int rowCnt = accTable->rowCount();
    accTable->insertRow(rowCnt);
    QTableWidgetItem *item1 = new QTableWidgetItem;
    QTableWidgetItem *item2 = new QTableWidgetItem;
    QTableWidgetItem *item3 = new QTableWidgetItem;
    QTableWidgetItem *item4 = new QTableWidgetItem;
    QTableWidgetItem *item5 = new QTableWidgetItem;
    QTableWidgetItem *item6 = new QTableWidgetItem;
    QTableWidgetItem *item7 = new QTableWidgetItem;
    item1->setText(QString::number(a.id));
    item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
    item2->setText(a.username);
    item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
    item3->setText(a.name);
    item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
    item4->setText(a.phone);
    item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
    item5->setText(a.email);
    item5->setFlags(item5->flags() & (~Qt::ItemIsEditable));
    item6->setText(a.status==ONLINE?"在线":"离线");
    item6->setFlags(item6->flags() & (~Qt::ItemIsEditable));
    item7->setText(a.last_time);
    item7->setFlags(item7->flags() & (~Qt::ItemIsEditable));

    accTable->setItem(rowCnt, 0, item1);
    accTable->setItem(rowCnt, 1, item2);
    accTable->setItem(rowCnt, 2, item3);
    accTable->setItem(rowCnt, 3, item4);
    accTable->setItem(rowCnt, 4, item5);
    accTable->setItem(rowCnt, 5, item6);
    accTable->setItem(rowCnt, 6, item7);
    accTable->setCurrentCell(rowCnt, 0);
}

// 用户注销
void AccWidget::signOut(int id)
{
    qDebug() << "sign out";
    for(int i = 0; i < accounts.size(); ++i)
    {
        if(accounts[i].id == id)
        {
            accounts[i].status = OFFLINE;
            accTable->item(i, 5)->setText("离线");
            emit sendAccounts(&accounts);

            vector<string> acc;
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back(QString::number(OFFLINE).toStdString());
            acc.push_back("");
            if(dbsql->Alter(TABLE_ACCOUNT, ACCOUNT_ID, QString::number(id).toStdString(), acc) != 0)
                qDebug() << "ERROR on alter";
            break;
        }
    }
}

//修改信息
void AccWidget::changeInfo(Account a)
{
    qDebug() << "change info";
    for(int i = 0; i < accounts.size(); ++i)
    {
        if(accounts[i].id == a.id)
        {
            accounts[i] = a;
            accTable->item(i, 2)->setText(a.name);
            accTable->item(i, 3)->setText(a.phone);
            accTable->item(i, 4)->setText(a.email);
            emit sendAccounts(&accounts);
            break;
        }
    }

    vector<string> acc;
    acc.push_back("");
    acc.push_back("");
    acc.push_back("");
    acc.push_back("");
    acc.push_back(a.name.toStdString());
    acc.push_back(a.phone.toStdString());
    acc.push_back(a.email.toStdString());
    acc.push_back("");
    acc.push_back("");
    if(dbsql->Alter(TABLE_ACCOUNT, ACCOUNT_ID, QString::number(a.id).toStdString(), acc) != 0)
        qDebug() << "ERROR on alter";

}

void AccWidget::driverWork(int id)
{
    for(int i = 0; i < accounts.size(); ++i)
    {
        if(accounts[i].id == id)
        {
            accounts[i].status = WORK;
            accTable->item(i, 5)->setText("执行任务中");
            emit sendAccounts(&accounts);

            vector<string> info;
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back(QString::number(accounts[i].status).toStdString());
            info.push_back("");
            if(dbsql->Alter(TABLE_ACCOUNT, ACCOUNT_ID, QString::number(accounts[i].id).toStdString(), info) != 0)
            {
                QMessageBox::warning(this, tr("错误"), tr("无法修改账户状态！"));
            }
            break;
        }
    }
}

void AccWidget::driverFree(int id)
{
    for(int i = 0; i < accounts.size(); ++i)
    {
        if(accounts[i].id == id)
        {
            accounts[i].status = ONLINE;
            accTable->item(i, 5)->setText("在线");
            emit sendAccounts(&accounts);

            vector<string> info;
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back(QString::number(accounts[i].status).toStdString());
            info.push_back("");
            if(dbsql->Alter(TABLE_ACCOUNT, ACCOUNT_ID, QString::number(accounts[i].id).toStdString(), info) != 0)
            {
                QMessageBox::warning(this, tr("错误"), tr("无法修改账户状态！"));
            }
            break;
        }
    }
}
