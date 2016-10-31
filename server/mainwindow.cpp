#include "mainwindow.h"
#include "tabwidget.h"
#include "mapviewer/mapwidget.h"
#include "deliverywidget.h"
#include "truckwidget.h"
#include "inventorywidget.h"
#include <QPalette>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
//    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(800, 800);
    setAutoFillBackground(true);

    tabWidget = new TabWidget(this);
    mapWidget = new MapWidget(this);
    deliveryWidget = new DeliveryWidget(this);
    truckWidget = new TruckWidget(this);
    inventoryWidget = new InventoryWidget(this);
    setCentralWidget(tabWidget);

    tabWidget->insertTab(0, mapWidget, "地图", "Maps");
    tabWidget->setTabEnabled(0, true);
    tabWidget->insertTab(1, deliveryWidget, "配送", "Delivery");
    tabWidget->setTabEnabled(1, true);
    tabWidget->insertTab(2, inventoryWidget, "货物", "Inventory");
    tabWidget->setTabEnabled(2, true);
    tabWidget->insertTab(3, truckWidget, "车辆", "Transfer truck");
    tabWidget->setTabEnabled(3, true);

    tabWidget->setCurrentIndex(0);

    connect(deliveryWidget, SIGNAL(selectMode()), this, SLOT(enterSelectMode()));
    connect(deliveryWidget, SIGNAL(cargoMode()), this, SLOT(enterCargoMode()));
    connect(mapWidget, SIGNAL(transferData(QVector<long>&, QVector<long>&, QStringList&)),
            this, SLOT(transferData(QVector<long>&, QVector<long>&, QStringList&)));
    connect(deliveryWidget, SIGNAL(drawPath(DeliveryPath*)), mapWidget, SLOT(addPath(DeliveryPath*)));
    connect(deliveryWidget, SIGNAL(removePath(int)), mapWidget, SLOT(removePath(int)));
    connect(deliveryWidget, SIGNAL(updatePath(int,DeliveryPath*)), mapWidget, SLOT(updatePath(int,DeliveryPath*)));
    connect(deliveryWidget, SIGNAL(truckGo(QString&)), truckWidget, SLOT(truckGo(QString&)));
    connect(deliveryWidget, SIGNAL(truckBack(QString&)), truckWidget, SLOT(truckBack(QString&)));
    connect(inventoryWidget, SIGNAL(sendTitles(QStringList&)), this, SLOT(sendTitles(QStringList&)));
    connect(truckWidget, SIGNAL(addTruck(TruckInfo*)), deliveryWidget, SLOT(addTruck(TruckInfo*)));
    connect(truckWidget, SIGNAL(delTruck(int)), deliveryWidget, SLOT(delTruck(int)));
}

// 进入选点模式
void MainWindow::enterSelectMode()
{
    tabWidget->setCurrentIndex(0);
    mapWidget->setSelectMode(true);
}

void MainWindow::enterCargoMode()
{
    tabWidget->setCurrentIndex(2);
    inventoryWidget->setSelectMode(true);
}

//传递数据并切换界面
void MainWindow::transferData(QVector<long> &points, QVector<long> &path, QStringList &nameList)
{
    tabWidget->setCurrentIndex(1);
    deliveryWidget->setPath(points, path, nameList);
}

void MainWindow::sendTitles(QStringList &titles)
{
    tabWidget->setCurrentIndex(1);
    deliveryWidget->setCargo(titles);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
/*
    QPalette p;
     QPixmap pic(":/images/background.jpg");
     p.setBrush(QPalette::Background,
                QBrush(pic.scaled(rect().width(),rect().height())));
     setPalette(p);
*/
}
