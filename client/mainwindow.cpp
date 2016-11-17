#include "mainwindow.h"
#include "mapview.h"
#include "socket.h"
#include "pathwidget.h"
#include "aboutdlg.h"
#include "userwidget.h"
#include "taskwidget.h"
#include <QFont>
#include <QToolButton>
#include <QPushButton>
#include <QBoxLayout>

MainWindow::MainWindow(QWidget *parent, ClientSocket *cli)
    : QMainWindow(parent)
    , client(cli)
{
    setWindowTitle(tr("QExpress"));
    setMinimumSize(800, 600);

    view = new MapView(this, cli);
    pathWidget = new PathWidget(this);
    zoomInBtn = new QPushButton(this);
    zoomOutBtn = new QPushButton(this);
    orderBtn = new QToolButton(this);
    trafficBtn = new QToolButton(this);
    userBtn = new QToolButton(this);
    aboutBtn = new QToolButton(this);

    zoomInBtn->setText("+");
    zoomOutBtn->setText("-");
    orderBtn->setText("任务");
    trafficBtn->setText("路况");
    zoomInBtn->setFixedSize(24,24);
    zoomOutBtn->setFixedSize(24,24);/*
    orderBtn->setFixedSize(32,32);
    trafficBtn->setFixedSize(32,32);
    userBtn->setFixedSize(64,64);
    aboutBtn->setFixedSize(32,32);*/
    orderBtn->setIconSize(QSize(24,24));
    trafficBtn->setIconSize(QSize(24,24));
    userBtn->setIconSize(QSize(64,64));
    aboutBtn->setIconSize(QSize(32, 32));
    orderBtn->setCursor(Qt::PointingHandCursor);
    trafficBtn->setCursor(Qt::PointingHandCursor);
    userBtn->setCursor(Qt::PointingHandCursor);
    aboutBtn->setCursor(Qt::PointingHandCursor);
    orderBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    trafficBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    orderBtn->setIcon(QIcon(":/images/order_32.png"));
    trafficBtn->setIcon(QIcon(":/images/traffic_32.png"));
    userBtn->setIcon(QIcon(":/images/user_64.png"));
    aboutBtn->setIcon(QIcon(":/images/about_48.png"));
    orderBtn->setStyleSheet("QToolButton{padding:2px}");
    trafficBtn->setStyleSheet("QToolButton{padding:2px}");
    userBtn->setStyleSheet("QToolButton{border:0}");
    aboutBtn->setStyleSheet("QToolButton{border:0}");

    QVBoxLayout *btnLayout = new QVBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setMargin(0);
    btnLayout->addWidget(zoomInBtn);
    btnLayout->addWidget(zoomOutBtn);
    QHBoxLayout *toolLayout = new QHBoxLayout;
    toolLayout->setSpacing(0);
    toolLayout->setMargin(0);
    toolLayout->addWidget(orderBtn);
    toolLayout->addWidget(trafficBtn);
    toolLayout->addSpacing(10);
    toolLayout->addWidget(userBtn);
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(0);
    gridLayout->setMargin(10);
    gridLayout->addWidget(pathWidget, 0, 0, Qt::AlignLeft|Qt::AlignTop);
    pathWidget->setStyleSheet("border:5px solid gray");
    gridLayout->addLayout(toolLayout, 0, 1, Qt::AlignRight|Qt::AlignTop);
    gridLayout->addWidget(aboutBtn, 1, 0, Qt::AlignLeft|Qt::AlignBottom);
    gridLayout->addLayout(btnLayout, 1, 1, Qt::AlignRight|Qt::AlignBottom);
    view->setLayout(gridLayout);

    setCentralWidget(view);

    connect(zoomInBtn, SIGNAL(clicked(bool)), view, SLOT(zoomIn()));
    connect(zoomOutBtn, SIGNAL(clicked(bool)), view, SLOT(zoomOut()));
    connect(orderBtn, SIGNAL(clicked(bool)), this, SLOT(getOrder()));
    connect(trafficBtn, SIGNAL(clicked(bool)), this, SLOT(UploadTraffic()));
    connect(userBtn, SIGNAL(clicked(bool)), this, SLOT(userSetting()));
    connect(aboutBtn, SIGNAL(clicked(bool)), this, SLOT(about()));

}

void MainWindow::getOrder()
{

}

void MainWindow::UploadTraffic()
{

}

void MainWindow::userSetting()
{

}

void MainWindow::about()
{
    AboutDlg *dlg = new AboutDlg(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}
