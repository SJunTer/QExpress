#include "mainwindow.h"
#include "mapview.h"
#include "socket.h"
#include "pathwidget.h"
#include "userdlg.h"
#include <QFont>
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
    orderBtn = new QPushButton(this);
    trafficBtn = new QPushButton(this);
    userBtn = new QPushButton(this);
    aboutBtn = new QPushButton(this);

    QFont font;
    font.setPixelSize(15);
    zoomInBtn->setFont(font);
    zoomOutBtn->setFont(font);
    orderBtn->setFont(font);
    trafficBtn->setFont(font);
    userBtn->setFont(font);
    aboutBtn->setFont(font);
    zoomInBtn->setText("+");
    zoomOutBtn->setText("-");
    orderBtn->setText("指令");
    trafficBtn->setText("路况");
    userBtn->setText("用户");
    aboutBtn->setText("关于");
    zoomInBtn->setFixedSize(BTN_SIZE,BTN_SIZE);
    zoomOutBtn->setFixedSize(BTN_SIZE,BTN_SIZE);
    orderBtn->setFixedSize(60,35);
    trafficBtn->setFixedSize(60,35);
    userBtn->setFixedSize(60,35);
    aboutBtn->setFixedSize(60,35);

    QVBoxLayout *btnLayout = new QVBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setMargin(0);
    btnLayout->addWidget(zoomInBtn);
    btnLayout->addWidget(zoomOutBtn);
    QHBoxLayout *toolLayout = new QHBoxLayout;
    toolLayout->setSpacing(5);
    toolLayout->setMargin(0);
    toolLayout->addWidget(orderBtn);
    toolLayout->addWidget(trafficBtn);
    toolLayout->addWidget(userBtn);
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(0);
    gridLayout->setMargin(10);
    gridLayout->addWidget(pathWidget, 0, 0, Qt::AlignLeft|Qt::AlignTop);
    gridLayout->addLayout(toolLayout, 0, 1, Qt::AlignRight|Qt::AlignTop);
    gridLayout->addWidget(aboutBtn, 1, 0, Qt::AlignLeft|Qt::AlignBottom);
    gridLayout->addLayout(btnLayout, 1, 1, Qt::AlignRight|Qt::AlignBottom);
    view->setLayout(gridLayout);

    setCentralWidget(view);

    connect(zoomInBtn, SIGNAL(clicked(bool)), view, SLOT(zoomIn()));
    connect(zoomOutBtn, SIGNAL(clicked(bool)), view, SLOT(zoomOut()));


}
