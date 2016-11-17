#include "mainwindow.h"
#include "tabwidget.h"
#include "mapviewer/mapwidget.h"
#include "deliverywidget.h"
#include "truckwidget.h"
#include "inventorywidget.h"
#include "accwidget.h"
#include "server/tcpserver.h"
#include <QThread>
#include <QCloseEvent>
#include <QMessageBox>
//#include <QPalette>
#include <QDebug>


#define MAPWIDGET_INDEX 0   // 地图
#define DELYWIDGET_INDEX 1   // 配送
#define ACCWIDGET_INDEX 2   // 帐号
#define TRKWIDGET_INDEX 3   // 货车
#define INVYWIDGET_INDEX 4   // 库存


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
//    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(1100, 800);
//    setAutoFillBackground(true);


    tabWidget = new TabWidget(this);
    mapWidget = new MapWidget(this);
    deliveryWidget = new DeliveryWidget(this);
    accWidget = new AccWidget(this);
    truckWidget = new TruckWidget(this);
    inventoryWidget = new InventoryWidget(this);

    runServer();

    setCentralWidget(tabWidget);

    tabWidget->insertTab(MAPWIDGET_INDEX, mapWidget, "地图", "Maps");
    tabWidget->setTabEnabled(MAPWIDGET_INDEX, true);
    tabWidget->insertTab(DELYWIDGET_INDEX, deliveryWidget, "配送", "Delivery");
    tabWidget->setTabEnabled(DELYWIDGET_INDEX, true);
    tabWidget->insertTab(ACCWIDGET_INDEX, accWidget, "账户", "Account");
    tabWidget->setTabEnabled(ACCWIDGET_INDEX, true);
    tabWidget->insertTab(TRKWIDGET_INDEX, truckWidget, "车辆", "Transfer truck");
    tabWidget->setTabEnabled(TRKWIDGET_INDEX, true);
    tabWidget->insertTab(INVYWIDGET_INDEX, inventoryWidget, "货物", "Inventory");
    tabWidget->setTabEnabled(INVYWIDGET_INDEX, true);
    /*************客户端管理界面*****************/


    tabWidget->setCurrentIndex(0);

    connect(mapWidget, SIGNAL(transferData(QVector<long>&, QVector<long>&, QStringList&)),
            this, SLOT(transferData(QVector<long>&, QVector<long>&, QStringList&)));

    connect(deliveryWidget, SIGNAL(selectMode()), this, SLOT(enterSelectMode()));
    connect(deliveryWidget, SIGNAL(cargoMode()), this, SLOT(enterCargoMode()));
    connect(deliveryWidget, SIGNAL(drawPath(DeliveryPath*)), mapWidget, SLOT(addPath(DeliveryPath*)));
    connect(deliveryWidget, SIGNAL(removePath(int)), mapWidget, SLOT(removePath(int)));
    connect(deliveryWidget, SIGNAL(updatePath(int,DeliveryPath*)), mapWidget, SLOT(updatePath(int,DeliveryPath*)));
    connect(deliveryWidget, SIGNAL(truckGo(QString&)), truckWidget, SLOT(truckGo(QString&)));
    connect(deliveryWidget, SIGNAL(truckBack(QString&)), truckWidget, SLOT(truckBack(QString&)));

    connect(inventoryWidget, SIGNAL(sendTitles(QStringList&)), this, SLOT(sendTitles(QStringList&)));

    connect(truckWidget, SIGNAL(addTruck(TruckInfo*)), deliveryWidget, SLOT(addTruck(TruckInfo*)));
    connect(truckWidget, SIGNAL(delTruck(int)), deliveryWidget, SLOT(delTruck(int)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::runServer()
{
    QThread *thread = new QThread;
    server = new TcpServer(mapWidget->view);
    server->moveToThread(thread);
    connect(thread, SIGNAL(started()), server, SLOT(start()));
    connect(server, SIGNAL(taskFinished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), server, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(this, SIGNAL(stopServer()), server, SLOT(stop()), Qt::DirectConnection);
    // 服务器进程和主进程通信
    connect(server, SIGNAL(signIn(int)), accWidget, SLOT(signIn(int)));
    connect(server, SIGNAL(signUp(Account&)), accWidget, SLOT(signUp(Account&)));
    connect(server, SIGNAL(signOut(int)), accWidget, SLOT(signOut(int)));
    thread->start(); // 开启服务器线程
}

// 进入选点模式
void MainWindow::enterSelectMode()
{
    tabWidget->setCurrentIndex(MAPWIDGET_INDEX);
    mapWidget->enterSelectMode();
}

void MainWindow::enterCargoMode()
{
    tabWidget->setCurrentIndex(INVYWIDGET_INDEX);
    inventoryWidget->setSelectMode(true);
}

//传递数据并切换界面
void MainWindow::transferData(QVector<long> &points, QVector<long> &path, QStringList &nameList)
{
    tabWidget->setCurrentIndex(DELYWIDGET_INDEX);
    deliveryWidget->setPath(points, path, nameList);
}


void MainWindow::sendTitles(QStringList &titles)
{
    tabWidget->setCurrentIndex(DELYWIDGET_INDEX);
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

// 退出提示
void MainWindow::closeEvent(QCloseEvent *e)
{
    if(QMessageBox::warning(this, tr("关闭"), tr("后台程序正在运行，确定退出吗？"),
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
    {
        e->ignore();
    }
    else
    {
        emit stopServer();
        e->accept();
    }
}
