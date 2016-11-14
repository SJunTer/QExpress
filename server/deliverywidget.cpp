#include "deliverywidget.h"
#include "deliverypath.h"
#include "nofocusdelegate.h"
#include "truckwidget.h"

#include <QPainter>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>
#include <QStyleOption>
#include <QBoxLayout>
#include <QStringList>
#include <QFont>
#include <QHeaderView>
#include <QMessageBox>
#include <QComboBox>
#include <QDebug>

DeliveryWidget::DeliveryWidget(QWidget *parent)
    : QWidget(parent)
    , editMode(false)
{
    deliveryTable = new QTableWidget(this);
    pathList = new QListWidget(this);
    addBtn = new QPushButton(this);
    delBtn = new QPushButton(this);
    selectBtn = new QPushButton(this);
    cargoBtn = new QPushButton(this);
    okBtn = new QPushButton(this);
    cancelBtn = new QPushButton(this);

    initTable();

    QFont font;
    font.setPixelSize(14);
    addBtn->setText(tr("添加"));
    addBtn->setFixedSize(60,35);
    addBtn->setFont(font);
    delBtn->setText(tr("删除"));
    delBtn->setFixedSize(60,35);
    delBtn->setFont(font);
    selectBtn->setText(tr("地图选点"));
    selectBtn->setFixedSize(100,35);
    selectBtn->setFont(font);
    cargoBtn->setText(tr("选择货物"));
    cargoBtn->setFixedSize(100, 35);
    cargoBtn->setFont(font);
    okBtn->setText(tr("确定配送"));
    okBtn->setFixedSize(100,35);
    okBtn->setFont(font);
    cancelBtn->setText(tr("取消配送"));
    cancelBtn->setFixedSize(100,35);
    cancelBtn->setFont(font);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setSpacing(15);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addWidget(deliveryTable);
    topLayout->addWidget(pathList);
    topLayout->setStretchFactor(deliveryTable, 5);
    topLayout->setStretchFactor(pathList, 2);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(0, 5, 0, 0);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(selectBtn);
    btnLayout->addWidget(cargoBtn);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(20);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    connect(addBtn, SIGNAL(clicked(bool)), this, SLOT(addLine()));
    connect(delBtn, SIGNAL(clicked(bool)), this, SLOT(delLine()));
    connect(selectBtn, SIGNAL(clicked(bool)), this, SLOT(selectPoints()));
    connect(cargoBtn, SIGNAL(clicked(bool)), this, SLOT(selectCargo()));
    connect(okBtn, SIGNAL(clicked(bool)), this, SLOT(applyPath()));
    connect(cancelBtn, SIGNAL(clicked(bool)), this, SLOT(cancel()));
    connect(deliveryTable, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(showPath(int,int,int,int)));
}

void DeliveryWidget::setCargo(QStringList &titles)
{
    int index = deliveryTable->currentRow();
    QString s;
    for(auto t : titles)
    {
        s += QString("%1;").arg(t);
    }
    deliveryTable->item(index, 2)->setText(s);
}

/*****************************************
 * set TableWidget Attributes
 * *************************************/
void DeliveryWidget::initTable()
{

    deliveryTable->setColumnCount(5);
    deliveryTable->horizontalHeader()->setDefaultSectionSize(150);
    deliveryTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header << tr("序号") << tr("车辆编号") << tr("运送货物") << tr("出发时间") << tr("当前状态");
    deliveryTable->setHorizontalHeaderLabels(header);

    QFont font = deliveryTable->horizontalHeader()->font();
    font.setBold(true);
    deliveryTable->horizontalHeader()->setFont(font);

    deliveryTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    deliveryTable->verticalHeader()->setDefaultSectionSize(20); //设置行高
    deliveryTable->verticalHeader()->setVisible(false); //设置垂直头不可见
//    deliveryTable->setFrameShape(QFrame::NoFrame); //设置无边框
//    deliveryTable->setShowGrid(false); //设置不显示格子线
    deliveryTable->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只可选择一个单元格
    deliveryTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
//    deliveryTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    deliveryTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    deliveryTable->horizontalHeader()->resizeSection(0,50); //设置表头第一列的宽度为50
    deliveryTable->horizontalHeader()->setFixedHeight(30); //设置表头的高度
//    deliveryTable->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
//    deliveryTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
    NoFocusDelegate nfd;
    deliveryTable->setItemDelegate(&nfd);  // 消除选中虚框
}


void DeliveryWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
/*
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    */
}

// 槽函数，添加一条新配送路径
void DeliveryWidget::addLine()
{
    if(editMode)
        return;

    editMode = true;
    tempPath = new DeliveryPath;

    int rowCnt = deliveryTable->rowCount();
    deliveryTable->insertRow(rowCnt);

    QTableWidgetItem *item1 = new QTableWidgetItem;
    QComboBox *item2 = new QComboBox;
    QTableWidgetItem *item3 = new QTableWidgetItem;
    QTableWidgetItem *item4 = new QTableWidgetItem;
    QTableWidgetItem *item5 = new QTableWidgetItem;

    item1->setText(QString::number(rowCnt+1));
    item1->setTextAlignment(Qt::AlignCenter);
    item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
    for(int i = 0; i < trucks.size(); ++i)
    {
        if(trucks[i]->m_isFree)
            item2->addItem(QString::number(trucks[i]->m_id+1));
    }
    item3->setText("");
    item3->setTextAlignment(Qt::AlignCenter);
    item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
    item4->setText("NULL");
    item4->setTextAlignment(Qt::AlignCenter);
    item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
    item5->setText("NULL");
    item5->setTextAlignment(Qt::AlignCenter);
    item5->setFlags(item5->flags() & (~Qt::ItemIsEditable));

    deliveryTable->setItem(rowCnt, 0, item1);
    deliveryTable->setCellWidget(rowCnt, 1, item2);
    deliveryTable->setItem(rowCnt, 2, item3);
    deliveryTable->setItem(rowCnt, 3, item4);
    deliveryTable->setItem(rowCnt, 4, item5);
    deliveryTable->setCurrentCell(rowCnt, 0);
}

// 槽函数，删除指定路径
void DeliveryWidget::delLine()
{
    int index = deliveryTable->currentRow();
    if(editMode || index == -1)
        return;

    deliveryTable->removeRow(index);
    DeliveryPath *p = paths.takeAt(index);
    delete p;
    emit removePath(index);
/*    for(int i = index; i < paths.size(); ++i)
    {
        qDebug()<<paths[i]->m_index += 1;
        deliveryTable->item(i,0)->setText(QString::number(i+1));
    }*/
}

// 接受选择按钮发送clicked信号
void DeliveryWidget::selectPoints()
{
    if(!editMode)
        return;

    emit selectMode();
}

void DeliveryWidget::selectCargo()
{
    if(!editMode)
        return;

    emit cargoMode();
}

//应用当前配送方案
void DeliveryWidget::applyPath()
{
    if(!editMode)
        return;

    int index = deliveryTable->currentRow();
    tempPath->m_truck =static_cast<QComboBox*>(deliveryTable->cellWidget(index, 1))->currentText();
    tempPath->m_cargo = deliveryTable->item(index, 2)->text();

    //信息不完善则弹窗提示
    if(tempPath->m_truck.size() == 0 || tempPath->m_cargo.size() == 0 ||
       tempPath->m_path.empty())
    {
        QMessageBox::warning(this, tr("警告"), tr("请完善信息后重试！"));
        return;
    }

    //设置随机时间表
    tempPath->randomSchedule(paths.size());
    connect(tempPath, SIGNAL(posChange(int,int)), this, SLOT(posChanged(int,int)));
    paths.append(tempPath);
    emit drawPath(tempPath);
    posChanged(0 ,deliveryTable->currentRow());
    emit truckGo(tempPath->m_truck);
    deliveryTable->cellWidget(index, 1)->setEnabled(false);
    editMode = false;
    QMessageBox::information(this, tr("提示"), tr("添加成功！"));
}

void DeliveryWidget::cancel()
{
    if(!editMode)
        return;

    tempPath->clear();
    int index = deliveryTable->currentRow();
    deliveryTable->removeRow(index);
    editMode = false;
}

void DeliveryWidget::showPath(DeliveryPath *p)
{
    pathList->clear();
    for(auto d : p->m_points)
    {
        pathList->addItem(d.name);
    }
}

void DeliveryWidget::updateCombo()
{
    int index = deliveryTable->rowCount()-1;
    if(index == -1)
        return;
    QComboBox *b = static_cast<QComboBox*>(deliveryTable->cellWidget(index, 1));
    b->clear();
    for(int i = 0; i < trucks.size(); ++i)
    {
        if(trucks[i]->m_isFree)
            b->addItem(QString::number(trucks[i]->m_id+1));
    }
    //b->setCurrentIndex(0);
}

void DeliveryWidget::showPath(int curRow, int curCol, int preRow, int preCol)
{
    Q_UNUSED(curCol);
    Q_UNUSED(preCol);
    if(curRow == -1)
        pathList->clear();
    else if(curRow == preRow)
        return;
    else if(curRow == paths.size())
        showPath(tempPath);
    else
        showPath(paths.at(curRow));
}

// 用于连接返回最佳路径的槽函数
void DeliveryWidget::setPath(QVector<long> &points, QVector<long> &path, QStringList &nameList)
{
    tempPath->setData(points, path, nameList);
    // 刷新路径列表
    showPath(tempPath);
}

void DeliveryWidget::posChanged(int pos, int index)
{
    Q_UNUSED(pos);
    if(pos == 1)
        deliveryTable->item(index, 3)->setText(paths[index]->startTime);
    deliveryTable->item(index, 4)->setText(paths[index]->m_status);
    emit updatePath(index, paths[index]);
//    qDebug() << pos <<  paths[index]->m_path.size();
    if(pos == paths[index]->m_path.size())
        emit truckBack(paths[index]->m_truck);
}

void DeliveryWidget::addTruck(TruckInfo *truck)
{
    trucks.append(truck);
    updateCombo();
}

void DeliveryWidget::delTruck(int index)
{
    trucks.takeAt(index);
    updateCombo();
}
