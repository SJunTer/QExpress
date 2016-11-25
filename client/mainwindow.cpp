#include "mainwindow.h"
#include "mapview.h"
#include "../network/socket.h"
#include "deliverypath.h"
#include "taskdlg.h"
#include "uploaddlg.h"
#include "userdlg.h"
#include "aboutdlg.h"
#include <QFont>
#include <QCloseEvent>
#include <QToolButton>
#include <QPushButton>
#include <QMessageBox>
#include <QBoxLayout>

MainWindow::MainWindow(QWidget *parent, ClientSocket *cli) :
    QMainWindow(parent),
    client(cli),
    uploadDlgShowed(false),
    userDlgShowed(false),
    taskRun(false)
{
    setWindowTitle(tr("QExpress"));
    setMinimumSize(800, 600);

    taskDlg = new TaskDlg(client, this);
    aboutDlg = new AboutDlg(this);

    view = new MapView(this, client);
    zoomInBtn = new QPushButton(this);
    zoomOutBtn = new QPushButton(this);
    taskBtn = new QToolButton(this);
    uploadBtn = new QToolButton(this);
    userBtn = new QToolButton(this);
    aboutBtn = new QToolButton(this);

    zoomInBtn->setText("+");
    zoomOutBtn->setText("-");
    taskBtn->setText("任务");
    uploadBtn->setText("路况");
    zoomInBtn->setFixedSize(24,24);
    zoomOutBtn->setFixedSize(24,24);
    taskBtn->setIconSize(QSize(24,24));
    uploadBtn->setIconSize(QSize(24,24));
    userBtn->setIconSize(QSize(64,64));
    aboutBtn->setIconSize(QSize(32, 32));
    taskBtn->setCursor(Qt::PointingHandCursor);
    uploadBtn->setCursor(Qt::PointingHandCursor);
    userBtn->setCursor(Qt::PointingHandCursor);
    aboutBtn->setCursor(Qt::PointingHandCursor);
    taskBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    uploadBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    taskBtn->setIcon(QIcon(":/images/order_32.png"));
    uploadBtn->setIcon(QIcon(":/images/traffic_32.png"));
    userBtn->setIcon(QIcon(":/images/user_64.png"));
    aboutBtn->setIcon(QIcon(":/images/about_48.png"));
    taskBtn->setStyleSheet("QToolButton{padding:2px}");
    uploadBtn->setStyleSheet("QToolButton{padding:2px}");
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
    toolLayout->addWidget(taskBtn);
    toolLayout->addWidget(uploadBtn);
    toolLayout->addSpacing(10);
    toolLayout->addWidget(userBtn);
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(0);
    gridLayout->setMargin(10);
    gridLayout->addLayout(toolLayout, 0, 1, Qt::AlignRight|Qt::AlignTop);
    gridLayout->addWidget(aboutBtn, 1, 0, Qt::AlignLeft|Qt::AlignBottom);
    gridLayout->addLayout(btnLayout, 1, 1, Qt::AlignRight|Qt::AlignBottom);
    view->setLayout(gridLayout);

    setCentralWidget(view);

    connect(zoomInBtn, SIGNAL(clicked(bool)), view, SLOT(zoomIn()));
    connect(zoomOutBtn, SIGNAL(clicked(bool)), view, SLOT(zoomOut()));
    connect(taskBtn, SIGNAL(clicked(bool)), this, SLOT(on_taskBtn_clicked()));
    connect(taskBtn, SIGNAL(clicked(bool)), taskDlg, SLOT(update()));
    connect(uploadBtn, SIGNAL(clicked(bool)), this, SLOT(on_uploadBtn_clicked()));
    connect(userBtn, SIGNAL(clicked(bool)), this, SLOT(on_userBtn_clicked()));
    connect(aboutBtn, SIGNAL(clicked(bool)), this, SLOT(on_aboutBtn_clicked()));

    connect(taskDlg, SIGNAL(drawPath(QList<Place>&)), view, SLOT(drawPath(QList<Place>&)));
    connect(taskDlg, SIGNAL(updatePath(int)), view, SLOT(updatePath(int)));
    connect(taskDlg, SIGNAL(endTask()), view, SLOT(cancelPath()));
    connect(taskDlg, SIGNAL(setTaskState(bool)), this, SLOT(setTaskState(bool)));
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if(taskRun)
    {
        QMessageBox::warning(this, tr("错误"), tr("任务进行中，无法退出！"), QMessageBox::Ok);
        e->ignore();
        return;
    }
    if(QMessageBox::warning(this, tr("关闭"), tr("是否确定退出程序？"),
                            QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
    {
        e->ignore();
    }
    else
    {
        e->accept();
    }
}

void MainWindow::on_taskBtn_clicked()
{
    taskDlg->show();
}

void MainWindow::on_uploadBtn_clicked()
{
    if(uploadDlgShowed)
        return;
    UploadDlg *uploadDlg = new UploadDlg(client, this);
    uploadDlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(uploadDlg, SIGNAL(closed()), this, SLOT(uploadDlgClosed()));
    uploadDlg->show();
    uploadDlgShowed = true;
}

void MainWindow::on_userBtn_clicked()
{
    if(userDlgShowed)
        return;
    UserDlg *userDlg = new UserDlg(client, this);
    userDlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(userDlg, SIGNAL(closed()), this, SLOT(userDlgClosed()));
    userDlg->show();
    userDlgShowed = true;
}

void MainWindow::on_aboutBtn_clicked()
{
    aboutDlg->show();
}
