#include "mapwidget.h"
#include "mapview.h"
#include "tilesdlg.h"
#include <QColor>
#include <QFont>
#include <QCheckBox>
#include <QListWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QListWidgetItem>
#include <QDebug>



MapWidget::MapWidget(QWidget *parent)
    : QMainWindow(parent)
{
    view = new MapView(this);
    tileBtn = new QPushButton(this);
    zoomInBtn = new QPushButton(this);
    zoomOutBtn = new QPushButton(this);

    QFont font;
    font.setPixelSize(15);
    tileBtn->setFont(font);
    zoomInBtn->setFont(font);
    zoomOutBtn->setFont(font);
    tileBtn->setText("切片工具");
    zoomInBtn->setText("+");
    zoomOutBtn->setText("-");
    tileBtn->setFixedSize(100,30);
    zoomInBtn->setFixedSize(BTN_SIZE,BTN_SIZE);
    zoomOutBtn->setFixedSize(BTN_SIZE,BTN_SIZE);

    QVBoxLayout *btnLayout = new QVBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setMargin(0);
    btnLayout->addWidget(zoomInBtn);
    btnLayout->addWidget(zoomOutBtn);
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(0);
    gridLayout->setMargin(10);
    gridLayout->addWidget(tileBtn, 0, 1, Qt::AlignRight|Qt::AlignTop);
    gridLayout->addLayout(btnLayout, 1, 1, Qt::AlignRight|Qt::AlignBottom);
    view->setLayout(gridLayout);

    connect(tileBtn, SIGNAL(clicked(bool)), this, SLOT(makeTile()));
    connect(zoomInBtn, SIGNAL(clicked(bool)), view, SLOT(zoomIn()));
    connect(zoomOutBtn, SIGNAL(clicked(bool)), view, SLOT(zoomOut()));

    setCentralWidget(view);

    connect(view, SIGNAL(selectDone(QList<Place>&)), this, SIGNAL(selectDone(QList<Place>&)));

}

void MapWidget::enterSelectMode() {
    view->setSelectMode(true);
}

void MapWidget::makeTile()
{
    TilesDlg *dlg = new TilesDlg(0, view->getScene());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(dlg, SIGNAL(setLayerVisible(int)), view, SLOT(setLayerVisible(int)));
    dlg->exec();
}

//////////路径相关///////////
void MapWidget::addPath(DeliveryPath *path)
{
    view->drawPath(path);
}

void MapWidget::updatePath(DeliveryPath *path)
{
    view->updatePath(path);
}

void MapWidget::removePath(int id)
{
    view->removePath(id);
}
