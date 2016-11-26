#include "truckwidget.h"
#include "nofocusdelegate.h"
#include "../security/des.h"
#include "database/operatesql.h"
#include "database/operatedefine.h"
#include <vector>
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
using namespace std;

TruckWidget::TruckWidget(Dbsql *d, QWidget *parent) :
    QWidget(parent),
    dbsql(d),
    editMode(false)
{

    truckTable = new QTableWidget(this);
    addBtn = new QPushButton(this);
    delBtn = new QPushButton(this);
    applyBtn = new QPushButton(this);
    initTable();

    addBtn->setText("添加");
    addBtn->setFixedSize(80, 35);
    delBtn->setText("删除");
    delBtn->setFixedSize(80, 35);
    applyBtn->setText("应用");
    applyBtn->setFixedSize(80, 35);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(10);
    layout->setMargin(20);
    layout->addWidget(truckTable);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(5);
    btnLayout->setMargin(0);
    btnLayout->addStretch();
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);
    btnLayout->addWidget(applyBtn);

    layout->addLayout(btnLayout);
    setLayout(layout);

    connect(addBtn, SIGNAL(clicked(bool)), this, SLOT(addRecord()));
    connect(delBtn, SIGNAL(clicked(bool)), this, SLOT(delRecord()));
    connect(applyBtn, SIGNAL(clicked(bool)), this, SLOT(applyRecord()));

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
    header << tr("序号") << tr("型号") << tr("车牌号") << tr("是否空闲");
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
    truckTable->setItemDelegate(new NoFocusDelegate(this));  // 消除选中虚框
}

void TruckWidget::readInfo()
{
    vector<string> ts;
    if(dbsql->AllSearch(TABLE_TRUCK, ts) != 0)
    {
        qDebug() << "ERROR on search";
        return;
    }
    for(unsigned i = 0; i < ts.size(); i+=TRUCK_COL_NUM)
    {
        TruckInfo *truck = new TruckInfo;
        truck->id = stoi(ts[i]);
        truck->brand = QString::fromStdString(ts[i+1]);
        truck->LPN = QString::fromStdString(ts[i+2]);
        truck->status = stoi(ts[i+3]);
        trucks.push_back(truck);

        int rowCnt = truckTable->rowCount();
        truckTable->insertRow(rowCnt);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QTableWidgetItem *item4 = new QTableWidgetItem;

        item1->setText(QString::number(truck->id));
        item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
        item2->setText(truck->brand);
        item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
        item3->setText(truck->LPN);
        item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
        item4->setText(truck->status==FREE?"空闲":"运行中");
        item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));

        truckTable->setItem(rowCnt, 0, item1);
        truckTable->setItem(rowCnt, 1, item2);
        truckTable->setItem(rowCnt, 2, item3);
        truckTable->setItem(rowCnt, 3, item4);
        truckTable->setCurrentCell(rowCnt, 0);
    }
    emit sendTrucks(&trucks);
}

void TruckWidget::addRecord()
{
    if(editMode)
    {
        QMessageBox::warning(this, tr("警告"), tr("请先应用新建项目！"));
        return;
    }

    editMode = true;

    temp = new TruckInfo;

    int rowCnt = truckTable->rowCount();
    if(trucks.empty())
        temp->id = 0;
    else
        temp->id = trucks.back()->id + 1;
    truckTable->insertRow(rowCnt);

    QTableWidgetItem *item1 = new QTableWidgetItem;
    QTableWidgetItem *item2 = new QTableWidgetItem;
    QTableWidgetItem *item3 = new QTableWidgetItem;
    QTableWidgetItem *item4 = new QTableWidgetItem;

    item1->setText(QString::number(temp->id));
    item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
    item2->setText("");
    item3->setText("");
    item4->setText("NULL");
    item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));

    truckTable->setItem(rowCnt, 0, item1);
    truckTable->setItem(rowCnt, 1, item2);
    truckTable->setItem(rowCnt, 2, item3);
    truckTable->setItem(rowCnt, 3, item4);
    truckTable->setCurrentCell(rowCnt, 0);

}

void TruckWidget::delRecord()
{
    int index = truckTable->currentRow();
    if(index == -1)
    {
        QMessageBox::warning(this, tr("错误"), tr("未选中项！"));
        return;
    }
    else if(index == trucks.size()) // 如果是编辑项
    {
        delete temp;
        truckTable->removeRow(truckTable->currentRow());
        editMode = false;
    }
    else
    {
        if(trucks.at(index)->status == MOVE)
        {
            QMessageBox::warning(this, tr("警告"), tr("车辆正在运行，无法删除！"));
            return;
        }
        if(QMessageBox::warning(this, "警告", "确定删除改信息吗？",
                                QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
            return;
        if(dbsql->Delete(TABLE_TRUCK, TRUCK_ID, QString::number(trucks[index]->id).toStdString()) != 0)
        {
            QMessageBox::warning(this, tr("错误"), tr("删除异常！"));
            return;
        }
        truckTable->removeRow(index);
        TruckInfo *t = trucks.takeAt(index);
        delete t;
        emit sendTrucks(&trucks);
        QMessageBox::information(this, tr("提示"), tr("删除成功！"));
    }
}

void TruckWidget::applyRecord()
{
    if(!editMode)
        return;

    int index = truckTable->rowCount()-1;
    temp->brand = truckTable->item(index, 1)->text();
    temp->LPN = truckTable->item(index, 2)->text();
    temp->status = FREE;
    if(temp->brand == "" || temp->LPN == "")
    {
        QMessageBox::warning(this, tr("警告"), tr("请完善信息后重试！"));
        return;
    }
    vector<string> truck;
    truck.push_back(QString::number(temp->id).toStdString());
    truck.push_back(temp->brand.toStdString());
    truck.push_back(temp->LPN.toStdString());
    truck.push_back(QString::number(temp->status).toStdString());
    if(dbsql->Insert(TABLE_TRUCK, truck) != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("添加数据异常！"));
        return;
    }
    truckTable->item(index, 3)->setText("空闲");
    for(int i = 1; i <= 2; ++i)
    {
        QTableWidgetItem *w = truckTable->item(index, i);
        w->setFlags(w->flags() & (~Qt::ItemIsEditable));
    }
    trucks.append(temp);
    editMode = false;
    emit sendTrucks(&trucks);
    QMessageBox::information(this, tr("提示"), tr("添加成功！"));
}

void TruckWidget::truckMove(int id)
{
    for(int i = 0; i < trucks.size(); ++i)
    {
        if(trucks[i]->id == id)
        {
            trucks[i]->status = MOVE;
            truckTable->item(i, 3)->setText("运行中");
            emit sendTrucks(&trucks);

            vector<string> info;
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back(QString::number(trucks[i]->status).toStdString());
            if(dbsql->Alter(TABLE_TRUCK, TRUCK_ID, QString::number(trucks[i]->id).toStdString(), info) != 0)
            {
                QMessageBox::warning(this, tr("错误"), tr("无法修改货车状态！"));
            }
            break;
        }
    }
}

void TruckWidget::truckFree(int id)
{
    for(int i = 0; i < trucks.size(); ++i)
    {
        if(trucks[i]->id == id)
        {
            trucks[i]->status = FREE;
            truckTable->item(i, 3)->setText("空闲");
            emit sendTrucks(&trucks);

            vector<string> info;
            info.push_back("");
            info.push_back("");
            info.push_back("");
            info.push_back(QString::number(trucks[i]->status).toStdString());
            if(dbsql->Alter(TABLE_TRUCK, TRUCK_ID, QString::number(trucks[i]->id).toStdString(), info) != 0)
            {
                QMessageBox::warning(this, tr("错误"), tr("无法修改货车状态！"));
            }
            break;
        }
    }
}
