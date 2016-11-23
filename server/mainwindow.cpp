#include "mainwindow.h"
#include "tabwidget.h"
#include "mapviewer/mapwidget.h"
#include "deliverywidget.h"
#include "truckwidget.h"
#include "cargowidget.h"
#include "accwidget.h"
#include "aboutdlg.h"
#include "messagedlg.h"
#include "server/tcpserver.h"
#include "database/operatesql.h"
#include <QThread>
#include <QCloseEvent>
#include <QToolButton>
#include <QBoxLayout>
#include <QMessageBox>
#include <QDockWidget>
#include <QDebug>


#define MAPWIDGET_INDEX 0   // 地图
#define DELYWIDGET_INDEX 1   // 配送
#define ACCWIDGET_INDEX 2   // 帐号
#define TRKWIDGET_INDEX 3   // 货车
#define INVYWIDGET_INDEX 4   // 库存


MainWindow::MainWindow(ServerSocket *s, Dbsql *d, QWidget *parent) :
    QMainWindow(parent),
    servSock(s),
    dbsql(d)
{
//    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(1100, 800);
//    setAutoFillBackground(true);


    tabWidget = new TabWidget(this);
    mapWidget = new MapWidget(this);
    deliveryWidget = new DeliveryWidget(this);
    accWidget = new AccWidget(dbsql, this);
    truckWidget = new TruckWidget(dbsql, this);
    inventoryWidget = new CargoWidget(dbsql, this);


    msgDlg = new MessageDlg(this);
    msgBtn = new QToolButton(this);
    msgBtn->setIcon(QIcon(":/images/msg_32.png"));
    msgBtn->setIconSize(QSize(32,32));
    msgBtn->setGeometry(1000,0, 32,32);
    msgBtn->setCursor(Qt::PointingHandCursor);
    aboutDlg = new AboutDlg(this);
    aboutBtn = new QToolButton(this);
    aboutBtn->setIcon(QIcon(":/images/about.png"));
    aboutBtn->setIconSize(QSize(32,32));
    aboutBtn->setGeometry(1040, 0, 32,32);
    aboutBtn->setCursor(Qt::PointingHandCursor);
    setStyleSheet("QToolButton{border:0}");
    connect(msgBtn, SIGNAL(clicked(bool)), this, SLOT(showMsgDlg()));
    connect(aboutBtn, SIGNAL(clicked(bool)), this, SLOT(showAboutDlg()));

    setCentralWidget(tabWidget);
    /*
    QDockWidget *dock = new QDockWidget(this);
    dock->setWidget(msgBtn);
    addDockWidget(Qt::RightDockWidgetArea, dock);*/

    runServer();

    tabWidget->insertTab(MAPWIDGET_INDEX, mapWidget, "地图", "Maps");
    tabWidget->setTabEnabled(MAPWIDGET_INDEX, true);
    tabWidget->insertTab(DELYWIDGET_INDEX, deliveryWidget, "配送", "Delivery");
    tabWidget->setTabEnabled(DELYWIDGET_INDEX, true);
    tabWidget->insertTab(ACCWIDGET_INDEX, accWidget, "账户", "Account");
    tabWidget->setTabEnabled(ACCWIDGET_INDEX, true);
    tabWidget->insertTab(TRKWIDGET_INDEX, truckWidget, "车辆", "Vehicle");
    tabWidget->setTabEnabled(TRKWIDGET_INDEX, true);
    tabWidget->insertTab(INVYWIDGET_INDEX, inventoryWidget, "货物", "Inventory");
    tabWidget->setTabEnabled(INVYWIDGET_INDEX, true);

    tabWidget->setCurrentIndex(0);

    connect(mapWidget, SIGNAL(transferData(QList<Place>&)), this, SLOT(transferData(QList<Place>&)));

    connect(deliveryWidget, SIGNAL(selectMode()), this, SLOT(enterSelectMode()));
    connect(deliveryWidget, SIGNAL(cargoMode()), this, SLOT(enterCargoMode()));
    connect(deliveryWidget, SIGNAL(drawPath(DeliveryPath*)), mapWidget, SLOT(addPath(DeliveryPath*)));
    connect(deliveryWidget, SIGNAL(removePath(int)), mapWidget, SLOT(removePath(int)));
    connect(deliveryWidget, SIGNAL(updatePath(int,DeliveryPath*)), mapWidget, SLOT(updatePath(int,DeliveryPath*)));
    connect(deliveryWidget, SIGNAL(truckGo(QString&)), truckWidget, SLOT(truckGo(QString&)));
    connect(deliveryWidget, SIGNAL(truckBack(QString&)), truckWidget, SLOT(truckBack(QString&)));
    connect(deliveryWidget, SIGNAL(sendTask(DeliveryPath*)), tcpServer, SIGNAL(sendTask(DeliveryPath*)));

    connect(inventoryWidget, SIGNAL(sendTitles(QStringList&)), this, SLOT(sendTitles(QStringList&)));

    connect(truckWidget, SIGNAL(addTruck(TruckInfo*)), deliveryWidget, SLOT(addTruck(TruckInfo*)));
    connect(truckWidget, SIGNAL(delTruck(int)), deliveryWidget, SLOT(delTruck(int)));

    connect(msgDlg, SIGNAL(signUp(Account)), accWidget, SLOT(signUp(Account)));

    /*-------------   读取信息   -----------------*/
    accWidget->readInfo();
    truckWidget->readInfo();
    inventoryWidget->readInfo();
}

MainWindow::~MainWindow()
{
    dbsql->CloseConnecion();
}

void MainWindow::runServer()
{
    QThread *thread = new QThread;
    tcpServer = new TcpServer(servSock, dbsql, mapWidget->view);
    tcpServer->moveToThread(thread);
    connect(thread, SIGNAL(started()), tcpServer, SLOT(start()));
    connect(tcpServer, SIGNAL(taskFinished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), tcpServer, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(this, SIGNAL(stopServer()), tcpServer, SLOT(stop()), Qt::DirectConnection);
    // 服务器进程和主进程通信
    connect(tcpServer, SIGNAL(initFail()), this, SLOT(initFail()));
    connect(tcpServer, SIGNAL(sg_signIn(int)), accWidget, SLOT(signIn(int)));
    connect(tcpServer, SIGNAL(sg_signUp(Account)), msgDlg, SLOT(signUpReq(Account)));
    connect(tcpServer, SIGNAL(sg_signOut(int)), accWidget, SLOT(signOut(int)));
    connect(tcpServer, SIGNAL(sg_changeInfo(Account)), accWidget, SLOT(changeInfo(Account)));

    connect(tcpServer, SIGNAL(sg_acptTask()), deliveryWidget, SLOT());
    connect(tcpServer, SIGNAL(sg_posChange()), deliveryWidget, SLOT());
    connect(tcpServer, SIGNAL(sg_taskFinish()), deliveryWidget, SLOT());
    connect(tcpServer, SIGNAL(sg_taskFail()), deliveryWidget, SLOT());

    connect(tcpServer, SIGNAL(sg_upload(QString,int,QString,QString)), msgDlg, SLOT(getUpload(QString,int,QString,QString)));

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
void MainWindow::transferData(QList<Place> &places)
{
    tabWidget->setCurrentIndex(DELYWIDGET_INDEX);
    deliveryWidget->setPath(places);
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

void MainWindow::initFail()
{
    QMessageBox::warning(this, tr("错误"), tr("服务器启动失败！"),
                            QMessageBox::Ok);
}

void MainWindow::showMsgDlg()
{
    msgDlg->show();
}

void MainWindow::showAboutDlg()
{
    aboutDlg->show();
}
