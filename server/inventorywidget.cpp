#include "inventorywidget.h"
#include "nofocusdelegate.h"
#include "../security/des.h"

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

InventoryWidget::InventoryWidget(QWidget *parent)
    : QWidget(parent)
    , selectMode(false)
    , editMode(false)
{
    cargoTable = new QTableWidget(this);
    addBtn = new QPushButton(this);
    delBtn = new QPushButton(this);
    applyBtn = new QPushButton(this);
    cancelBtn = new QPushButton(this);
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
    cancelBtn->setText("取消");
    cancelBtn->setFont(font);
    cancelBtn->setFixedSize(60, 35);
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
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(selectBtn);
    btnLayout->addSpacing(10);

    layout->addLayout(btnLayout);
    setLayout(layout);

    connect(addBtn, SIGNAL(clicked(bool)), this, SLOT(addRecord()));
    connect(delBtn, SIGNAL(clicked(bool)), this, SLOT(delRecord()));
    connect(applyBtn, SIGNAL(clicked(bool)), this, SLOT(applyRecord()));
    connect(cancelBtn, SIGNAL(clicked(bool)), this, SLOT(cancelRecord()));
    connect(selectBtn, SIGNAL(clicked(bool)), this, SLOT(selectCargo()));
}

InventoryWidget::~InventoryWidget()
{
    for(auto n : cargos)
    {
	CargoInfo *c = cargos.takeFirst();
	delete c;
    }
}

void InventoryWidget::initTable()
{
    cargoTable->setColumnCount(6);
    cargoTable->horizontalHeader()->setDefaultSectionSize(150);
    cargoTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header << tr("选择") << tr("编号") << tr("名称") << tr("目的地") << tr("收件人") << tr("当前状态");
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

void InventoryWidget::addRecord()
{
    if(editMode)
        return;
    editMode = true;

    temp = new CargoInfo;

     int rowCnt = cargoTable->rowCount();
     temp->m_index = rowCnt;
     cargoTable->insertRow(rowCnt);

     QCheckBox *item1 = new QCheckBox;
     QTableWidgetItem *item2 = new QTableWidgetItem;
     QTableWidgetItem *item3 = new QTableWidgetItem;
     QTableWidgetItem *item4 = new QTableWidgetItem;
     QTableWidgetItem *item5 = new QTableWidgetItem;
     QTableWidgetItem *item6 = new QTableWidgetItem;

     item1->setText("");
     item2->setText(QString::number(rowCnt+1));
     item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
     item3->setText(temp->m_descrip);
     item4->setText(temp->m_dest);
     item5->setText(temp->m_reciver);
     item6->setText("仓库中");
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

void InventoryWidget::delRecord()
{
    if(editMode)
        return;

    int index = cargoTable->currentRow();
    if(index == -1)
        return;
    if(cargos[index]->m_status == IsMove)
    {
        QMessageBox::warning(this, tr("警告"), tr("货物正在配送，无法删除！"));
        return;
    }
    if(cargos[index]->m_isSelect)
    {
        QMessageBox::warning(this, tr("警告"), tr("已选择该货物，无法删除！"));
        return;
    }
    cargoTable->removeRow(index);
    CargoInfo *c = cargos.takeAt(index);
    delete c;

}

void InventoryWidget::applyRecord()
{
    if(!editMode)
        return;

    int index = cargoTable->currentRow();
    temp->m_descrip = cargoTable->item(index, 2)->text();
    temp->m_dest = cargoTable->item(index, 3)->text();
    temp->m_reciver = cargoTable->item(index, 4)->text();
    if(temp->m_descrip == "" || temp->m_dest == "" || temp->m_reciver == "")
    {
        QMessageBox::warning(this, tr("警告"), tr("请完善信息后重试！"));
        return;
    }
    cargos.append(temp);
    editMode = false;
    QMessageBox::information(this, tr("提示"), tr("添加成功！"));
}

void InventoryWidget::cancelRecord()
{
    if(!editMode)
        return;

    delete temp;
    cargoTable->removeRow(cargoTable->currentRow());
    editMode = false;
}

void InventoryWidget::selectCargo()
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
        if(cargos[i]->m_isSelect && cargos[i]->m_status == InRepo)
        {
            // 设置该行不可标记
            cargoTable->cellWidget(i, 0)->setEnabled(false);
            for(int j = 1; j < cargoTable->columnCount(); ++j)
            {
                QTableWidgetItem *item = cargoTable->item(i, j);
                item->setFlags(item->flags() & (~Qt::ItemIsEditable));
            }
            cargos[i]->m_status = IsMove;
            cargoTable->item(i, 5)->setText("配送中");
            titles << cargos[i]->m_descrip;
        }
    }
    emit sendTitles(titles);
    selectMode = false;
}


void InventoryWidget::complete(int index)
{
    cargos[index]->m_status = IsComplete;
    cargoTable->item(index, 5)->setText("配送完成");
}
