#include "truckwidget.h"
#include "nofocusdelegate.h"
#include "../security/des.h"

#include <string>
#include <sstream>
#include <QTableWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QStringList>
#include <QFont>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDebug>

TruckWidget::TruckWidget(QWidget *parent)
    : QWidget(parent)
    , editMode(false)
{

    truckTable = new QTableWidget(this);
    addBtn = new QPushButton(this);
    delBtn = new QPushButton(this);
    applyBtn = new QPushButton(this);
    cancelBtn = new QPushButton(this);
    readBtn = new QPushButton(this);
    saveBtn = new QPushButton(this);
    initTable();

    QFont font;
    font.setPixelSize(15);
    addBtn->setText("添加");
    addBtn->setFont(font);
    addBtn->setFixedSize(60, 35);
    delBtn->setText("删除");
    delBtn->setFont(font);
    delBtn->setFixedSize(60, 35);
    applyBtn->setText("应用");
    applyBtn->setFont(font);
    applyBtn->setFixedSize(60, 35);
    cancelBtn->setText("取消");
    cancelBtn->setFont(font);
    cancelBtn->setFixedSize(60, 35);
    readBtn->setText("读取存档");
    readBtn->setFont(font);
    readBtn->setFixedSize(100, 35);
    saveBtn->setText("写入存档");
    saveBtn->setFont(font);
    saveBtn->setFixedSize(100, 35);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->addWidget(truckTable);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(0, 5, 0, 0);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);
    btnLayout->addWidget(applyBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(readBtn);
    btnLayout->addWidget(saveBtn);

    layout->addLayout(btnLayout);
    setLayout(layout);

    connect(addBtn, SIGNAL(clicked(bool)), this, SLOT(addRecord()));
    connect(delBtn, SIGNAL(clicked(bool)), this, SLOT(delRecord()));
    connect(applyBtn, SIGNAL(clicked(bool)), this, SLOT(applyRecord()));
    connect(cancelBtn, SIGNAL(clicked(bool)), this, SLOT(cancelRecord()));
    connect(readBtn, SIGNAL(clicked(bool)), this, SLOT(readRecord()));
    connect(saveBtn, SIGNAL(clicked(bool)), this, SLOT(saveRecord()));
}

TruckWidget::~TruckWidget()
{
    for(auto n : trucks)
    {
	TruckInfo *t = trucks.takeFirst();
	delete t;
    }
}

void TruckWidget::initTable()
{
    truckTable->setColumnCount(4);
    truckTable->horizontalHeader()->setDefaultSectionSize(150);
    truckTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header << tr("车辆编号") << tr("车辆型号") << tr("驾驶员") << tr("是否空闲");
    truckTable->setHorizontalHeaderLabels(header);

    QFont font = truckTable->horizontalHeader()->font();
    font.setBold(true);
    truckTable->horizontalHeader()->setFont(font);

    truckTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    truckTable->verticalHeader()->setDefaultSectionSize(20);
    truckTable->verticalHeader()->setVisible(false);
//    truckTable->setFrameShape(QFrame::NoFrame); //设置无边框
//    truckTable->setShowGrid(false); //设置不显示格子线
    truckTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只可选择一个单元格
    truckTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
//    truckTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    truckTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
//    truckTable->horizontalHeader()->resizeSection(0,50); //设置表头第一列的宽度为50
    truckTable->horizontalHeader()->setFixedHeight(30); //设置表头的高度
//    truckTable->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
//    truckTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
    NoFocusDelegate nfd;
    truckTable->setItemDelegate(&nfd);  // 消除选中虚框
}

void TruckWidget::addRecord()
{
    if(editMode)
        return;
    editMode = true;

    temp = new TruckInfo;

    int rowCnt = truckTable->rowCount();
    temp->m_id = rowCnt;
    truckTable->insertRow(rowCnt);

    QTableWidgetItem *item1 = new QTableWidgetItem;
    QTableWidgetItem *item2 = new QTableWidgetItem;
    QTableWidgetItem *item3 = new QTableWidgetItem;
    QTableWidgetItem *item4 = new QTableWidgetItem;

    item1->setText(QString::number(rowCnt+1));
    item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
    item2->setText(temp->m_brand);
    item3->setText(temp->m_driver);
    item4->setText(temp->m_isFree?"是":"否");
    item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));

    truckTable->setItem(rowCnt, 0, item1);
    truckTable->setItem(rowCnt, 1, item2);
    truckTable->setItem(rowCnt, 2, item3);
    truckTable->setItem(rowCnt, 3, item4);
    truckTable->setCurrentCell(rowCnt, 0);

}

void TruckWidget::delRecord()
{
    if(editMode)
        return;

    int index = truckTable->currentRow();
    if(index == -1)
        return;
    if(!trucks.at(index)->m_isFree)
    {
        QMessageBox::warning(this, tr("警告"), tr("车辆正在运行，请稍后再试！"));
        return;
    }
    truckTable->removeRow(index);
    TruckInfo *t = trucks.takeAt(index);
    delete t;
    emit delTruck(index);

    for(int i = index; i < trucks.size(); ++i)
    {
        trucks[i]->m_id += 1;
        truckTable->item(i,0)->setText(QString::number(i+1));
    }
}

void TruckWidget::applyRecord()
{
    if(!editMode)
        return;

    int index = truckTable->currentRow();
    temp->m_brand = truckTable->item(index, 1)->text();
    temp->m_driver = truckTable->item(index, 2)->text();
    if(temp->m_brand == "" || temp->m_driver == "")
    {
        QMessageBox::warning(this, tr("警告"), tr("请完善信息后重试！"));
        return;
    }
    trucks.append(temp);
    emit addTruck(temp);
    editMode = false;
    QMessageBox::information(this, tr("提示"), tr("添加成功！"));
}

void TruckWidget::cancelRecord()
{
    if(!editMode)
        return;

    delete temp;
    truckTable->removeRow(truckTable->currentRow());
    editMode = false;
}

void TruckWidget::readRecord()
{/*
    if(editMode)
        return;

    if(!trucks.empty())
    {
        if(QMessageBox::warning(this, tr("警告"), tr("读取存档将覆盖当前数据，是否继续？"),
                                QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
            return;

        for(int i = trucks.size()-1; i >= 0; --i)
        {
            truckTable->setCurrentCell(i,0);
            delRecord();
        }
    }

    std::string record;
    if(des->DES_Unlock(record, "truck.dat") == -1)
    {
        QMessageBox::warning(this, tr("警告"), tr("无法打开数据文件！"));
        return;
    }
    std::istringstream is(record);
    int id;
    bool b;
    std::string s1, s2;
    while(is >> id >> s1 >> s2 >> b)
    {
        TruckInfo *temp = new TruckInfo;
        trucks.append(temp);

        int rowCnt = truckTable->rowCount();
        temp->m_id = rowCnt;
        temp->m_brand = QString::fromStdString(s1);
        temp->m_driver = QString::fromStdString(s2);
        temp->m_isFree = b;
        truckTable->insertRow(rowCnt);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QTableWidgetItem *item4 = new QTableWidgetItem;

        item1->setText(QString::number(rowCnt+1));
        item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
        item2->setText(temp->m_brand);
        item3->setText(temp->m_driver);
        item4->setText(temp->m_isFree?"是":"否");
        item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));

        truckTable->setItem(rowCnt, 0, item1);
        truckTable->setItem(rowCnt, 1, item2);
        truckTable->setItem(rowCnt, 2, item3);
        truckTable->setItem(rowCnt, 3, item4);
        truckTable->setCurrentCell(rowCnt, 0);

        emit addTruck(temp);

    }
*/
}

void TruckWidget::saveRecord()
{/*
    if(editMode)
        return;

    for(auto &n : trucks)
    {
        if(!n->m_isFree)
        {
            QMessageBox::warning(this, tr("警告"), tr("当前有车辆在运行！"));
            return;
        }
    }

    QString s;
    for(auto &t : trucks)
        s += QString("%1 %2 %3 %4 ").arg(t->m_id).arg(t->m_brand).arg(t->m_driver).arg(t->m_isFree);
    des->DES_Lock(s.toStdString().c_str(), "truck.dat");

    QMessageBox::information(this, tr("提示"), tr("写入成功！"));*/
}

void TruckWidget::truckBack(QString &truckId)
{
    for(auto &t : trucks)
    {
        if(t->m_id+1 == truckId.toInt())
        {
            t->m_isFree = true;
            truckTable->item(t->m_id, 3)->setText("是");
            return;
        }
    }
}

void TruckWidget::truckGo(QString &truckId)
{
    for(auto &t : trucks)
    {
        if(t->m_id+1 == truckId.toInt())
        {
            if(!t->m_isFree)
                break;
            t->m_isFree = false;
            truckTable->item(t->m_id, 3)->setText("否");
            break;
        }
    }
}
