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
    deliveryWidget = new DeliveryWidget(dbsql, this);
    accWidget = new AccWidget(dbsql, this);
    truckWidget = new TruckWidget(dbsql, this);
    cargoWidget = new CargoWidget(dbsql, this);


    msgDlg = new MessageDlg(this);
    msgBtn = new QToolButton(this);
    msgBtn->setIcon(QIcon(":/images/msg_32.png"));
    msgBtn->setIconSize(QSize(32,32));
    msgBtn->setGeometry(1000,20, 32,32);
    msgBtn->setCursor(Qt::PointingHandCursor);
    aboutDlg = new AboutDlg(this);
    aboutBtn = new QToolButton(this);
    aboutBtn->setIcon(QIcon(":/images/about.png"));
    aboutBtn->setIconSize(QSize(32,32));
    aboutBtn->setGeometry(1040, 20, 32,32);
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
    tabWidget->insertTab(INVYWIDGET_INDEX, cargoWidget, "货物", "Inventory");
    tabWidget->setTabEnabled(INVYWIDGET_INDEX, true);

    tabWidget->setCurrentIndex(0);

    connect(mapWidget, SIGNAL(selectDone(QList<Place>&)), this, SLOT(selectDone(QList<Place>&)));
    connect(deliveryWidget, SIGNAL(selectMode()), this, SLOT(enterSelectMode()));

    connect(deliveryWidget, SIGNAL(drawPath(DeliveryPath*)), mapWidget, SLOT(addPath(DeliveryPath*)));
    connect(deliveryWidget, SIGNAL(updatePath(DeliveryPath*)), mapWidget, SLOT(updatePath(DeliveryPath*)));
    connect(deliveryWidget, SIGNAL(removePath(int)), mapWidget, SLOT(removePath(int)));

    connect(deliveryWidget, SIGNAL(driverWork(int)), accWidget, SLOT(driverWork(int)));
    connect(deliveryWidget, SIGNAL(driverFree(int)), accWidget, SLOT(driverFree(int)));
    connect(deliveryWidget, SIGNAL(cargoMove(QStringList)), cargoWidget, SLOT(cargoMove(QStringList)));
    connect(deliveryWidget, SIGNAL(cargoReach(QStringList)), cargoWidget, SLOT(cargoReach(QStringList)));
    connect(deliveryWidget, SIGNAL(cargoFail(QStringList)), cargoWidget, SLOT(cargofail(QStringList)));
    connect(deliveryWidget, SIGNAL(truckMove(int)), truckWidget, SLOT(truckMove(int)));
    connect(deliveryWidget, SIGNAL(truckFree(int)), truckWidget, SLOT(truckFree(int)));

    connect(accWidget, SIGNAL(sendAccounts(QList<Account>*)), deliveryWidget, SLOT(setAccounts(QList<Account>*)));
    connect(cargoWidget, SIGNAL(sendCargos(QList<CargoInfo*>*)), deliveryWidget, SLOT(setCargos(QList<CargoInfo*>*)));
    connect(truckWidget, SIGNAL(sendTrucks(QList<TruckInfo*>*)), deliveryWidget, SLOT(setTrucks(QList<TruckInfo*>*)));

    connect(msgDlg, SIGNAL(signUp(Account)), accWidget, SLOT(signUp(Account)));
    connect(deliveryWidget, SIGNAL(taskMsg(QString&)), msgDlg, SLOT(taskMsg(QString&)));

    /*-------------   读取信息   -----------------*/
    deliveryWidget->readInfo();
    accWidget->readInfo();
    truckWidget->readInfo();
    cargoWidget->readInfo();
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
    connect(accWidget, SIGNAL(closeClient(int)), tcpServer, SIGNAL(closeClient(int)), Qt::DirectConnection);
    connect(deliveryWidget, SIGNAL(sendTask(DeliveryPath)), tcpServer, SIGNAL(sendTask(DeliveryPath)), Qt::DirectConnection);
    // 服务器进程和主进程通信
    connect(tcpServer, SIGNAL(sg_signIn(int)), accWidget, SLOT(signIn(int)));
    connect(tcpServer, SIGNAL(sg_signUp(Account)), msgDlg, SLOT(signUpReq(Account)));
    connect(tcpServer, SIGNAL(sg_signOut(int)), accWidget, SLOT(signOut(int)));
    connect(tcpServer, SIGNAL(sg_changeInfo(Account)), accWidget, SLOT(changeInfo(Account)));

    connect(tcpServer, SIGNAL(sg_acptTask(int)), deliveryWidget, SLOT(acptTask(int)));
    connect(tcpServer, SIGNAL(sg_posChange(int,int)), deliveryWidget, SLOT(posChanged(int,int)));
    connect(tcpServer, SIGNAL(sg_taskFinish(int)), deliveryWidget, SLOT(taskFinish(int)));
    connect(tcpServer, SIGNAL(sg_taskFail(int)), deliveryWidget, SLOT(taskFail(int)));

    connect(tcpServer, SIGNAL(sg_upload(QString,int,QString,QString)), msgDlg, SLOT(getUpload(QString,int,QString,QString)));

    thread->start(); // 开启服务器线程
}


// 进入选点模式
void MainWindow::enterSelectMode()
{
    tabWidget->setCurrentIndex(MAPWIDGET_INDEX);
    mapWidget->enterSelectMode();
}


//传递数据并切换界面
void MainWindow::selectDone(QList<Place> &places)
{
    tabWidget->setCurrentIndex(DELYWIDGET_INDEX);
    deliveryWidget->setPath(places);
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


void MainWindow::showMsgDlg()
{
    msgDlg->show();
}

void MainWindow::showAboutDlg()
{
    aboutDlg->show();
}
