#include "accwidget.h"
#include "nofocusdelegate.h"
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QBoxLayout>
#include <QDebug>


AccWidget::AccWidget(QWidget *parent)
    : QWidget(parent)
{
    accTable = new QTableWidget(this);
    addBtn = new QPushButton(this);
    delBtn = new QPushButton(this);
    sendBtn = new QPushButton(this);
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
    sendBtn->setText("发送消息");
    sendBtn->setFixedSize(60, 35);
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
    btnLayout->addWidget(sendBtn);
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    connect(addBtn, SIGNAL(clicked(bool)), this, SLOT(addAccount()));
    connect(delBtn, SIGNAL(clicked(bool)), this, SLOT(delAccount()));
    connect(sendBtn, SIGNAL(clicked(bool)), this, SLOT(sendMsg()));
    connect(closeBtn, SIGNAL(clicked(bool)), this, SLOT(closeConn()));

}

AccWidget::~AccWidget()
{
    for(auto &n : accounts)
    {
        Account *a = accounts.takeFirst();
        delete a;
    }
}

//初始化表窗体样式
void AccWidget::initTable()
{

    accTable->setColumnCount(7);
    accTable->horizontalHeader()->setDefaultSectionSize(150);
    accTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header << tr("用户名") << tr("密码") << tr("姓名")
           << tr("电话") << tr("邮箱") << tr("当前状态") << tr("最后登陆时间");
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

void AccWidget::signIn(int id)
{
    qDebug() << "sign in" << id;
    for(int i = 0; i < accounts.size(); ++i)
    {
        if(accounts[i]->id == id)
            accounts[i]->status = ONLINE;
    }
}

void AccWidget::signOut(int id)
{
    qDebug() << "sign out" << id;
    for(int i = 0; i < accounts.size(); ++i)
    {
        if(accounts[i]->id == id)
            accounts[i]->status = OFFLINE;
    }
}
