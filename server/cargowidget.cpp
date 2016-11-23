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
    selectMode(false),
    editMode(false)
{
    cargoTable = new QTableWidget(this);
    addBtn = new QPushButton(this);
    delBtn = new QPushButton(this);
    applyBtn = new QPushButton(this);
    selectBtn = new QPushButton(this);

    initTable();

    QFont font;
    font.setPixelSize(13);
    addBtn->setText("添加");
    addBtn->setFont(font);
    addBtn->setFixedSize(60, 35);
    delBtn->setText("删除");
    delBtn->setFont(font);
    delBtn->setFixedSize(60, 35);
    applyBtn->setText("应用");
    applyBtn->setFont(font);
    applyBtn->setFixedSize(60, 35);
    selectBtn->setText("选择");
    selectBtn->setFont(font);
    selectBtn->setFixedSize(60, 35);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->addWidget(cargoTable);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(0, 5, 0, 0);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);
    btnLayout->addWidget(applyBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(selectBtn);
    btnLayout->addSpacing(10);

    layout->addLayout(btnLayout);
    setLayout(layout);

    connect(addBtn, SIGNAL(clicked(bool)), this, SLOT(addRecord()));
    connect(delBtn, SIGNAL(clicked(bool)), this, SLOT(delRecord()));
    connect(applyBtn, SIGNAL(clicked(bool)), this, SLOT(applyRecord()));
    connect(selectBtn, SIGNAL(clicked(bool)), this, SLOT(selectCargo()));

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
    cargoTable->setColumnCount(6);
    cargoTable->horizontalHeader()->setDefaultSectionSize(150);
    cargoTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header << tr("选择") << tr("序号") << tr("名称") << tr("目的地") << tr("收件人") << tr("当前状态");
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
    cargoTable->horizontalHeader()->resizeSection(0,50); //设置表头第一列的宽度为50
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

        QCheckBox *item1 = new QCheckBox;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;
        QTableWidgetItem *item4 = new QTableWidgetItem;
        QTableWidgetItem *item5 = new QTableWidgetItem;
        QTableWidgetItem *item6 = new QTableWidgetItem;

        item1->setText("");
        item2->setText(QString::number(cargo->id));
        item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
        item3->setText(cargo->descrip);
        item4->setText(cargo->dest);
        item5->setText(cargo->reciver);
        switch(cargo->status)
        {
        case InRepo: item6->setText("仓库中"); break;
        case IsMove: item6->setText("配送中"); break;
        case IsComplete: item6->setText("配送完成"); break;
        }
        item6->setFlags(item6->flags() & (~Qt::ItemIsEditable));

        cargoTable->setCellWidget(rowCnt, 0, item1);
        cargoTable->setItem(rowCnt, 1, item2);
        cargoTable->setItem(rowCnt, 2, item3);
        cargoTable->setItem(rowCnt, 3, item4);
        cargoTable->setItem(rowCnt, 4, item5);
        cargoTable->setItem(rowCnt, 5, item6);
        cargoTable->setCurrentCell(rowCnt, 0);
        connect(item1, SIGNAL(stateChanged(int)), cargo, SLOT(stateChanged(int)));
    }
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

     QCheckBox *item1 = new QCheckBox;
     QTableWidgetItem *item2 = new QTableWidgetItem;
     QTableWidgetItem *item3 = new QTableWidgetItem;
     QTableWidgetItem *item4 = new QTableWidgetItem;
     QTableWidgetItem *item5 = new QTableWidgetItem;
     QTableWidgetItem *item6 = new QTableWidgetItem;

     item1->setText("");
     item2->setText(QString::number(temp->id));
     item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
     item3->setText("");
     item4->setText("");
     item5->setText("");
     item6->setText("NULL");
     item6->setFlags(item6->flags() & (~Qt::ItemIsEditable));

     cargoTable->setCellWidget(rowCnt, 0, item1);
     cargoTable->setItem(rowCnt, 1, item2);
     cargoTable->setItem(rowCnt, 2, item3);
     cargoTable->setItem(rowCnt, 3, item4);
     cargoTable->setItem(rowCnt, 4, item5);
     cargoTable->setItem(rowCnt, 5, item6);
     cargoTable->setCurrentCell(rowCnt, 0);
     connect(item1, SIGNAL(stateChanged(int)), temp, SLOT(stateChanged(int)));
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
        if(cargos[index]->m_isSelect)
        {
            QMessageBox::warning(this, tr("警告"), tr("已选择该货物，无法删除！"));
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
        QMessageBox::information(this, tr("提示"), tr("删除成功！"));
    }
}

void CargoWidget::applyRecord()
{
    if(!editMode)
        return;

    int i = cargoTable->currentRow();
    temp->descrip = cargoTable->item(i, 2)->text();
    temp->dest = cargoTable->item(i, 3)->text();
    temp->reciver = cargoTable->item(i, 4)->text();
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
    cargoTable->item(i, 5)->setText("仓库中");
    editMode = false;
    QMessageBox::information(this, tr("提示"), tr("添加成功！"));
}


void CargoWidget::selectCargo()
{
    if(!selectMode)
        return;
    if(editMode)
    {
        QMessageBox::warning(this, tr("警告"), tr("请先完成编辑货物！"));
        return;
    }

    for(int i = 0; i < cargos.size(); ++i)
    {
        if(cargos[i]->m_isSelect && cargos[i]->status == InRepo)
        {
            vector<string> cargo;
            cargo.push_back("");
            cargo.push_back("");
            cargo.push_back("");
            cargo.push_back("");
            cargo.push_back(QString::number(cargos[i]->status).toStdString());
            if(dbsql->Alter(TABLE_CARGO, CARGO_ID, QString::number(cargos[i]->id).toStdString(), cargo) != 0)
            {
                QMessageBox::warning(this, tr("错误"), tr("无法修改货物状态！"));
                return;
            }

            // 设置该行不可标记
            cargoTable->cellWidget(i, 0)->setEnabled(false);
            for(int j = 1; j < cargoTable->columnCount(); ++j)
            {
                QTableWidgetItem *item = cargoTable->item(i, j);
                item->setFlags(item->flags() & (~Qt::ItemIsEditable));
            }
            cargos[i]->status = IsMove;
            cargoTable->item(i, 5)->setText("配送中");
            titles << cargos[i]->descrip;
        }
    }
    emit sendTitles(titles);
    selectMode = false;
}


void CargoWidget::complete(int i)
{
    cargos[i]->status = IsComplete;
    cargoTable->item(i, 5)->setText("配送完成");

    vector<string> cargo;
    cargo.push_back("");
    cargo.push_back("");
    cargo.push_back("");
    cargo.push_back("");
    cargo.push_back(QString::number(cargos[i]->status).toStdString());
    if(dbsql->Alter(TABLE_CARGO, CARGO_ID, QString::number(cargos[i]->id).toStdString(), cargo) != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("无法修改货物状态！"));
    }
}
