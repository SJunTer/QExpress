#include "cargowidget.h"
#include "nofocusdelegate.h"
#include "../security/des.h"
#include "database/operatesql.h"
#include "database/operatedefine.h"
#include <vector>
#include <string>
#include <sstream>
#include <QFont>
#include <QCheckBox>
#include <QBoxLayout>
#include <QStringList>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDebug>
using namespace std;

CargoWidget::CargoWidget(Dbsql *d, QWidget *parent) :
    QWidget(parent),
    dbsql(d),
    editMode(false)
{
    cargoTable = new QTableWidget(this);
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
    layout->setContentsMargins(20, 20, 20, 20);
    layout->addWidget(cargoTable);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(5);
    btnLayout->setContentsMargins(0, 0, 0, 0);
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

CargoWidget::~CargoWidget()
{
    for(auto n : cargos)
    {
	CargoInfo *c = cargos.takeFirst();
	delete c;
    }
}

void CargoWidget::initTable()
{
    cargoTable->setColumnCount(5);
    cargoTable->horizontalHeader()->setDefaultSectionSize(150);
    cargoTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header  << tr("序号") << tr("名称") << tr("目的地") << tr("收件人") << tr("当前状态");
    cargoTable->setHorizontalHeaderLabels(header);

    QFont font = cargoTable->horizontalHeader()->font();
    font.setBold(true);
    cargoTable->horizontalHeader()->setFont(font);

    cargoTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    cargoTable->verticalHeader()->setDefaultSectionSize(20); //设置行高
    cargoTable->verticalHeader()->setVisible(false); //设置垂直头不可见
//    cargoTable->setFrameShape(QFrame::NoFrame); //设置无边框
//    cargoTable->setShowGrid(false); //设置不显示格子线
    cargoTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只可选择一个单元格
    cargoTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
//    cargoTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    cargoTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
//    cargoTable->horizontalHeader()->resizeSection(0,50); //设置表头第一列的宽度为50
    cargoTable->horizontalHeader()->setFixedHeight(30); //设置表头的高度
//    cargoTable->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
//    cargoTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
    cargoTable->setItemDelegate(new NoFocusDelegate(this));  // 消除选中虚框

}

void CargoWidget::readInfo()
{
    vector<string> cs;
    if(dbsql->AllSearch(TABLE_CARGO, cs) != 0)
    {
        qDebug() << "ERROR on search";
        return;
    }
    for(unsigned i = 0; i < cs.size(); i+=CARGO_COL_NUM)
    {
        CargoInfo *cargo = new CargoInfo;
        cargo->id = stoi(cs[i]);
        cargo->descrip = QString::fromStdString(cs[i+1]);
        cargo->dest = QString::fromStdString(cs[i+2]);
        cargo->reciver = QString::fromStdString(cs[i+3]);
        cargo->status = stoi(cs[i+4]);
        cargos.push_back(cargo);

        int rowCnt = cargoTable->rowCount();
        cargoTable->insertRow(rowCnt);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QTableWidgetItem *item4 = new QTableWidgetItem;
        QTableWidgetItem *item5 = new QTableWidgetItem;

        item1->setText(QString::number(cargo->id));
        item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
        item2->setText(cargo->descrip);
        item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
        item3->setText(cargo->dest);
        item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
        item4->setText(cargo->reciver);
        item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
        switch(cargo->status)
        {
        case InRepo: item5->setText("仓库中"); break;
        case IsMove: item5->setText("配送中"); break;
        case IsComplete: item5->setText("配送完成"); break;
        }
        item5->setFlags(item5->flags() & (~Qt::ItemIsEditable));

        cargoTable->setItem(rowCnt, 0, item1);
        cargoTable->setItem(rowCnt, 1, item2);
        cargoTable->setItem(rowCnt, 2, item3);
        cargoTable->setItem(rowCnt, 3, item4);
        cargoTable->setItem(rowCnt, 4, item5);
        cargoTable->setCurrentCell(rowCnt, 0);
    }
    emit sendCargos(&cargos);
}

void CargoWidget::addRecord()
{
    if(editMode)
    {
        QMessageBox::warning(this, tr("警告"), tr("请先应用新建项目！"));
        return;
    }
    editMode = true;

    temp = new CargoInfo;

     int rowCnt = cargoTable->rowCount();
     if(cargos.empty())
         temp->id = 0;
     else
         temp->id = cargos.back()->id+1;
     cargoTable->insertRow(rowCnt);

     QTableWidgetItem *item1 = new QTableWidgetItem;
     QTableWidgetItem *item2 = new QTableWidgetItem;
     QTableWidgetItem *item3 = new QTableWidgetItem;
     QTableWidgetItem *item4 = new QTableWidgetItem;
     QTableWidgetItem *item5 = new QTableWidgetItem;

     item1->setText(QString::number(temp->id));
     item1->setFlags(item2->flags() & (~Qt::ItemIsEditable));
     item2->setText("");
     item3->setText("");
     item4->setText("");
     item5->setText("NULL");
     item5->setFlags(item5->flags() & (~Qt::ItemIsEditable));

     cargoTable->setItem(rowCnt, 0, item1);
     cargoTable->setItem(rowCnt, 1, item2);
     cargoTable->setItem(rowCnt, 2, item3);
     cargoTable->setItem(rowCnt, 3, item4);
     cargoTable->setItem(rowCnt, 4, item5);
     cargoTable->setCurrentCell(rowCnt, 0);

}

void CargoWidget::delRecord()
{
    int index = cargoTable->currentRow();
    if(index == -1)
    {
        QMessageBox::warning(this, tr("错误"), tr("未选中项！"));
        return;
    }
    else if(index == cargos.size())
    {
        delete temp;
        cargoTable->removeRow(cargoTable->currentRow());
        editMode = false;
    }
    else
    {
        if(cargos[index]->status == IsMove)
        {
            QMessageBox::warning(this, tr("警告"), tr("货物正在配送，无法删除！"));
            return;
        }
        if(QMessageBox::warning(this, "警告", "确定删除改信息吗？",
                                QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
            return;
        if(dbsql->Delete(TABLE_CARGO, CARGO_ID, QString::number(cargos[index]->id).toStdString()) != 0)
        {
            QMessageBox::warning(this, tr("错误"), tr("删除异常！"));
            return;
        }
        cargoTable->removeRow(index);
        CargoInfo *c = cargos.takeAt(index);
        delete c;
        emit sendCargos(&cargos);
        QMessageBox::information(this, tr("提示"), tr("删除成功！"));
    }
}

void CargoWidget::applyRecord()
{
    if(!editMode)
        return;

    int index = cargoTable->rowCount()-1;
    temp->descrip = cargoTable->item(index, 1)->text();
    temp->dest = cargoTable->item(index, 2)->text();
    temp->reciver = cargoTable->item(index, 3)->text();
    temp->status = InRepo;
    if(temp->descrip == "" || temp->dest == "" || temp->reciver == "")
    {
        QMessageBox::warning(this, tr("警告"), tr("请完善信息后重试！"));
        return;
    }
    vector<string> cargo;
    cargo.push_back(QString::number(temp->id).toStdString());
    cargo.push_back(temp->descrip.toStdString());
    cargo.push_back(temp->dest.toStdString());
    cargo.push_back(temp->reciver.toStdString());
    cargo.push_back(QString::number(temp->status).toStdString());
    if(dbsql->Insert(TABLE_CARGO, cargo) != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("添加数据异常！"));
        return;
    }
    cargos.append(temp);
    cargoTable->item(index, 4)->setText("仓库中");
    for(int i = 1; i <= 3; ++i)
    {
        QTableWidgetItem *w = cargoTable->item(index, i);
        w->setFlags(w->flags() & (~Qt::ItemIsEditable));
    }
    editMode = false;
    emit sendCargos(&cargos);
    QMessageBox::information(this, tr("提示"), tr("添加成功！"));
}

void CargoWidget::cargoMove(QStringList cs)
{
    for(int i = 0; i < cs.size(); ++i)
    {
        for(int j = 0; j < cargos.size(); ++j)
        {
            if(cs[i] == cargos[j]->descrip)
            {
                cargos[j]->status = IsMove;
                cargoTable->item(j, 4)->setText("配送中");
                emit sendCargos(&cargos);

                vector<string> cargo;
                cargo.push_back("");
                cargo.push_back("");
                cargo.push_back("");
                cargo.push_back("");
                cargo.push_back(QString::number(cargos[j]->status).toStdString());
                if(dbsql->Alter(TABLE_CARGO, CARGO_ID, QString::number(cargos[j]->id).toStdString(), cargo) != 0)
                {
                    QMessageBox::warning(this, tr("错误"), tr("无法修改货物状态！"));
                }
                break;
            }
        }// 查找对应货物
    }
}

void CargoWidget::cargoReach(QStringList cs)
{
    for(int i = 0; i < cs.size(); ++i)
    {
        for(int j = 0; j < cargos.size(); ++j)
        {
            if(cs[i] == cargos[j]->descrip)
            {
                cargos[j]->status = IsComplete;
                cargoTable->item(j, 4)->setText("配送完成");
                emit sendCargos(&cargos);

                vector<string> cargo;
                cargo.push_back("");
                cargo.push_back("");
                cargo.push_back("");
                cargo.push_back("");
                cargo.push_back(QString::number(cargos[j]->status).toStdString());
                if(dbsql->Alter(TABLE_CARGO, CARGO_ID, QString::number(cargos[j]->id).toStdString(), cargo) != 0)
                {
                    QMessageBox::warning(this, tr("错误"), tr("无法修改货物状态！"));
                }
                break;
            }
        }// 查找对应货物
    }
}

void CargoWidget::cargofail(QStringList cs)
{
    for(int i = 0; i < cs.size(); ++i)
    {
        for(int j = 0; j < cargos.size(); ++j)
        {
            if(cs[i] == cargos[j]->descrip)
            {
                cargos[j]->status = InRepo;
                cargoTable->item(j, 4)->setText("仓库中");
                emit sendCargos(&cargos);

                vector<string> cargo;
                cargo.push_back("");
                cargo.push_back("");
                cargo.push_back("");
                cargo.push_back("");
                cargo.push_back(QString::number(cargos[j]->status).toStdString());
                if(dbsql->Alter(TABLE_CARGO, CARGO_ID, QString::number(cargos[j]->id).toStdString(), cargo) != 0)
                {
                    QMessageBox::warning(this, tr("错误"), tr("无法修改货物状态！"));
                }
                break;
            }
        }// 查找对应货物
    }
}
