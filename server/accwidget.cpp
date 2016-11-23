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

    QFont font;
    font.setPixelSize(16);
    addBtn->setText("+");
    addBtn->setEnabled(false);
    addBtn->setFont(font);
    addBtn->setFixedSize(35,35);
    delBtn->setText("-");
    delBtn->setFixedSize(35,35);
    delBtn->setFont(font);
    closeBtn->setText("切断连接");
    closeBtn->setFixedSize(60,35);

    initTable();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(10);
    mainLayout->setMargin(20);
    mainLayout->addWidget(accTable);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setMargin(0);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);
    btnLayout->addStretch();
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
    accTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
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
        a.last_time = QTime::fromString(QString::fromStdString(accs[i+8]));
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
        item6->setText(a.status==ONLINE?"在线":"离线");
        item6->setFlags(item6->flags() & (~Qt::ItemIsEditable));
        item7->setText(a.last_time.toString());
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
}

void AccWidget::addAccount()
{

}

void AccWidget::delAccount()
{

}

void AccWidget::sendMsg()
{

}

void AccWidget::closeConn()
{

}

//用户登录
void AccWidget::signIn(int id)
{
    qDebug() << "sign in";
    for(int i = 0; i < accounts.size(); ++i)
    {
        if(accounts[i].id == id)
        {
            QTime t = QTime::currentTime();
            accounts[i].status = ONLINE;
            accounts[i].last_time = t;
            accTable->item(i, 5)->setText("上线");
            accTable->item(i, 6)->setText(t.toString());

            vector<string> acc;
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back("");
            acc.push_back(QString::number(ONLINE).toStdString());
            acc.push_back(accounts[i].last_time.toString().toStdString());
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
    item7->setText(a.last_time.toString());
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
