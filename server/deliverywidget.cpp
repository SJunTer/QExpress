#include "deliverywidget.h"
#include "delvdlg.h"
#include "nofocusdelegate.h"
#include "database/operatedefine.h"
#include "database/operatesql.h"
#include <vector>
#include <string>
#include <QFont>
#include <QPainter>
#include <QDateTime>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>
#include <QStyleOption>
#include <QBoxLayout>
#include <QStringList>
#include <QHeaderView>
#include <QMessageBox>
#include <QComboBox>
#include <QDebug>
using namespace std;

DeliveryWidget::DeliveryWidget(Dbsql *d, QWidget *parent)
    : QWidget(parent)
    , dbsql(d)
    , delvDlgShowed(false)
{
    deliveryTable = new QTableWidget(this);
    pathList = new QListWidget(this);
    addBtn = new QPushButton(this);
    delBtn = new QPushButton(this);

    initTable();

    addBtn->setText(tr("添加"));
    addBtn->setFixedSize(60,35);
    delBtn->setText(tr("删除"));
    delBtn->setFixedSize(60,35);

    pathList->setFixedWidth(300);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setSpacing(15);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addWidget(deliveryTable);
    topLayout->addWidget(pathList);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(0,0,0,0);
    btnLayout->addStretch();
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(10);
    mainLayout->setMargin(20);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    connect(addBtn, SIGNAL(clicked(bool)), this, SLOT(on_addBtn_clicked()));
    connect(delBtn, SIGNAL(clicked(bool)), this, SLOT(on_delBtn_clicked()));
    connect(deliveryTable, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(showPath(int,int,int,int)));
}


void DeliveryWidget::readInfo()
{
    vector<string> path;
     if(dbsql->AllSearch(TABLE_PATH, path) != 0)
     {
         qDebug() << "ERROR on search";
         return;
     }
     for(unsigned i = 0; i < path.size(); i+=PATH_COL_NUM)
     {
         DeliveryPath *p = new DeliveryPath;
         p->id = stoi(path[i]);
         p->truckId = stoi(path[i+1]);
         p->driverId = stoi(path[i+2]);
         QString cargos = QString::fromStdString(path[i+3]);
         p->cargos = cargos.split(';', QString::SkipEmptyParts);
         p->status = stoi(path[i+4]);
         QStringList places = QString::fromStdString(path[i+5]).split(';', QString::SkipEmptyParts);
         for(int i = 0; i < places.size(); ++i)
         {
             Place place;
             place.title = places[i];
             if(i == 0 || i == places.size()-1)
                 place.type = IsRepo;
             else
                 place.type = IsDely;
             p->places.append(place);
         }
         p->startTime = QString::fromStdString(path[i+6]);
         p->moveTime = QString::fromStdString(path[i+7]);
         paths.append(p);

         int rowCnt = deliveryTable->rowCount();
         deliveryTable->insertRow(rowCnt);

         QTableWidgetItem *item1 = new QTableWidgetItem;
         QTableWidgetItem *item2 = new QTableWidgetItem;
         QTableWidgetItem *item3 = new QTableWidgetItem;
         QTableWidgetItem *item4 = new QTableWidgetItem;
         QTableWidgetItem *item5 = new QTableWidgetItem;
         QTableWidgetItem *item6 = new QTableWidgetItem;

         item1->setText(QString::number(p->id));  //Id
         item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
         item2->setText(QString::number(p->truckId));         // 货车
         item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
         item3->setText(QString::number(p->driverId));         // 驾驶员
         item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
         item4->setText(cargos);         // 货物
         item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
         item5->setText(p->startTime);     //出发时间
         item5->setFlags(item5->flags() & (~Qt::ItemIsEditable));
         switch(p->status)
         {
         case WAIT: item6->setText("等待中"); break;
         case RUN: item6->setText("配送中"); break;
         case FINISH: item6->setText("配送完成"); break;
         case INTR: item6->setText("配送失败"); break;
         }
         item6->setFlags(item6->flags() & (~Qt::ItemIsEditable));

         deliveryTable->setItem(rowCnt, 0, item1);
         deliveryTable->setItem(rowCnt, 1, item2);
         deliveryTable->setItem(rowCnt, 2, item3);
         deliveryTable->setItem(rowCnt, 3, item4);
         deliveryTable->setItem(rowCnt, 4, item5);
         deliveryTable->setItem(rowCnt, 5, item6);
         deliveryTable->setCurrentCell(rowCnt, 0);
     }
}

// 设置表格样式
void DeliveryWidget::initTable()
{

    deliveryTable->setColumnCount(6);
    deliveryTable->horizontalHeader()->setDefaultSectionSize(150);
    deliveryTable->horizontalHeader()->setSectionsClickable(false);

    QStringList header;
    header << tr("序号") << tr("车牌号") << tr("驾驶员") << tr("运送货物") << tr("出发时间") << tr("当前状态");
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
//    deliveryTable->horizontalHeader()->resizeSection(0,50); //设置表头第一列的宽度为50
    deliveryTable->horizontalHeader()->setFixedHeight(30); //设置表头的高度
//    deliveryTable->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色
//    deliveryTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
    deliveryTable->setItemDelegate(new NoFocusDelegate(this));  // 消除选中虚框
}


// 弹窗选择信息
void DeliveryWidget::on_addBtn_clicked()
{
    if(delvDlgShowed)
        return;
    delvDlg = new DelvDlg(accounts, cargos, trucks, this);
    delvDlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(delvDlg, SIGNAL(addPath(DeliveryPath*)), this, SLOT(addPath(DeliveryPath*)));
    connect(delvDlg, SIGNAL(selectPoint()), this, SIGNAL(selectMode()));
    connect(delvDlg, SIGNAL(closed()), this, SLOT(closeDelvDLg()));
    delvDlg->show();
    delvDlgShowed = true;
}

// 槽函数，删除指定路径
void DeliveryWidget::on_delBtn_clicked()
{
    int index = deliveryTable->currentRow();
    if(index == -1)
        return;
    else
    {
        if(paths[index]->status == RUN)
        {
            QMessageBox::warning(this, "错误", "配送任务进行中，无法删除！",QMessageBox::Ok);
            return;
        }
        if(QMessageBox::warning(this, "警告", "删除后信息将无法恢复，是否继续？",
                                QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
            return;

        if(dbsql->Delete(TABLE_PATH, PATH_ID, QString::number(paths[index]->id).toStdString()) != 0)
        {
            QMessageBox::warning(this, tr("错误"), tr("删除异常！"));
            return;
        }
        deliveryTable->removeRow(index);
        emit removePath(paths[index]->id);
        DeliveryPath *p = paths.takeAt(index);
        delete p;
        QMessageBox::information(this, tr("提示"), tr("删除成功！"));
//       emit removePath(index);
    }

}


void DeliveryWidget::showPath(DeliveryPath *p)
{/*
    pathList->clear();
    for(auto d : p->m_points)
    {
        pathList->addItem(d.name);
    }*/
}

void DeliveryWidget::showPath(int curRow, int curCol, int preRow, int preCol)
{/*
    Q_UNUSED(curCol);
    Q_UNUSED(preCol);
    if(curRow == -1)
        pathList->clear();
    else if(curRow == preRow)
        return;
    else if(curRow == paths.size())
        showPath(tempPath);
    else
        showPath(paths.at(curRow));*/
}

//////////////////////////TODO/////////////////////
// 设置配送路径
void DeliveryWidget::setPath(QList<Place> &places)
{
    delvDlg->setPath(places);
    delvDlg->show();

    // 刷新路径列表
    //    showPath(tempPath);
}


//*************响应客户端****************//
void DeliveryWidget::acptTask(int id)
{
    for(int i = 0; i < paths.size(); ++i)
    {
        if(paths[i]->id == id)
        {
            paths[i]->startTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            paths[i]->status = RUN;
            deliveryTable->item(i, 4)->setText(paths[i]->startTime);
            deliveryTable->item(i, 5)->setText("配送中");

            vector<string> path;
            path.push_back("");
            path.push_back("");
            path.push_back("");
            path.push_back("");
            path.push_back(QString::number(paths[i]->status).toStdString());
            path.push_back("");
            path.push_back(paths[i]->startTime.toStdString());
            path.push_back("");
            if(dbsql->Alter(TABLE_PATH, PATH_ID, QString::number(id).toStdString(), path) != 0)
                qDebug() << "ERROR on alter";
            break;
        }
    }
}

void DeliveryWidget::posChanged(int id, int pos)
{
    for(int i = 0; i < paths.size(); ++i)
    {
        if(paths[i]->id == id)
        {
            paths[i]->pos = pos;
            emit updatePath(paths[i]);
        }
    }
}

void DeliveryWidget::taskFinish(int id)
{
    for(int i = 0; i < paths.size(); ++i)
    {
        if(paths[i]->id == id)
        {
            QDateTime start = QDateTime::fromString(paths[i]->startTime, "yyyy-MM-dd hh:mm:ss");
            QDateTime now = QDateTime::currentDateTime();
            paths[i]->moveTime = QString::number(start.secsTo(now));
            paths[i]->status = FINISH;
            deliveryTable->item(i, 5)->setText("配送完成");
            emit removePath(id);

            emit driverFree(paths[i]->driverId);
            emit cargoReach(paths[i]->cargos);
            emit truckFree(paths[i]->truckId);

            vector<string> path;
            path.push_back("");
            path.push_back("");
            path.push_back("");
            path.push_back("");
            path.push_back(QString::number(paths[i]->status).toStdString());
            path.push_back("");
            path.push_back("");
            path.push_back(paths[i]->moveTime.toStdString());
            if(dbsql->Alter(TABLE_PATH, PATH_ID, QString::number(id).toStdString(), path) != 0)
                qDebug() << "ERROR on alter";
            break;
        }
    }
}

void DeliveryWidget::taskFail(int id)
{
    for(int i = 0; i < paths.size(); ++i)
    {
        if(paths[i]->id == id)
        {
            QDateTime start = QDateTime::fromString(paths[i]->startTime, "yyyy-MM-dd hh:mm:ss");
            QDateTime now = QDateTime::currentDateTime();
            paths[i]->moveTime = QString::number(start.secsTo(now));
            paths[i]->status = INTR;
            deliveryTable->item(i, 5)->setText("配送失败");
            emit removePath(id);

            emit driverFree(paths[i]->driverId);
            emit cargoFail(paths[i]->cargos);
            emit truckFree(paths[i]->truckId);

            vector<string> path;
            path.push_back("");
            path.push_back("");
            path.push_back("");
            path.push_back("");
            path.push_back(QString::number(paths[i]->status).toStdString());
            path.push_back("");
            path.push_back("");
            path.push_back(paths[i]->moveTime.toStdString());
            if(dbsql->Alter(TABLE_PATH, PATH_ID, QString::number(id).toStdString(), path) != 0)
                qDebug() << "ERROR on alter";
            break;
        }
    }
}

void DeliveryWidget::closeDelvDLg()
{
    delvDlgShowed = false;
}

void DeliveryWidget::addPath(DeliveryPath *path)
{
    if(paths.empty())
        path->id = 0;
    else
        path->id = paths.back()->id+1;
    path->pos = 0;
    path->status = WAIT;

    vector<string> info;
    info.push_back(QString::number(path->id).toStdString());
    info.push_back(QString::number(path->truckId).toStdString());
    info.push_back(QString::number(path->driverId).toStdString());
    QString cargos;
    for(int i = 0; i < path->cargos.size(); ++i)
    {
        cargos += path->cargos[i];
        if(i != path->cargos.size()-1)
            cargos += "; ";
    }
    info.push_back(cargos.toStdString());
    info.push_back(QString::number(path->status).toStdString());
    QString places;
    for(int i = 0; i < path->places.size(); ++i)
    {
        if(path->places[i].type != IsPass)
        {
            places += path->places[i].title + ";";
        }
    }
    info.push_back(places.toStdString());
    info.push_back("");
    info.push_back("");
    if(dbsql->Insert(TABLE_PATH, info) != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("添加数据异常！"));
        return;
    }

    int rowCnt = deliveryTable->rowCount();
    deliveryTable->insertRow(rowCnt);

    QTableWidgetItem *item1 = new QTableWidgetItem;
    QTableWidgetItem *item2 = new QTableWidgetItem;
    QTableWidgetItem *item3 = new QTableWidgetItem;
    QTableWidgetItem *item4 = new QTableWidgetItem;
    QTableWidgetItem *item5 = new QTableWidgetItem;
    QTableWidgetItem *item6 = new QTableWidgetItem;

    item1->setText(QString::number(path->id));  //Id
    item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
    item2->setText(QString::number(path->truckId));         // 货车
    item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
    item3->setText(QString::number(path->driverId));         // 驾驶员
    item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
    item4->setText(cargos);         // 货物
    item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
    item5->setText("NULL");     //出发时间
    item5->setFlags(item5->flags() & (~Qt::ItemIsEditable));
    item6->setText("等待中");     //当前状态
    item6->setFlags(item6->flags() & (~Qt::ItemIsEditable));

    deliveryTable->setItem(rowCnt, 0, item1);
    deliveryTable->setItem(rowCnt, 1, item2);
    deliveryTable->setItem(rowCnt, 2, item3);
    deliveryTable->setItem(rowCnt, 3, item4);
    deliveryTable->setItem(rowCnt, 4, item5);
    deliveryTable->setItem(rowCnt, 5, item6);
    deliveryTable->setCurrentCell(rowCnt, 0);

    paths.append(path);
    emit drawPath(path);

    emit sendTask(*path);
    emit driverWork(path->driverId);
    emit cargoMove(path->cargos);
    emit truckMove(path->truckId);

    QMessageBox::information(this, tr("提示"), tr("添加成功！"));

}
