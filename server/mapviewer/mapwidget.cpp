#include "mapwidget.h"
#include "deliverypath.h"
#include "tilesdlg.h"
#include <QBoxLayout>
#include <QListWidgetItem>
#include <QColor>
#include <QFont>
#include <QDebug>



MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent)
{
    view = new MapView(this);
//    pathList = new QListWidget(this);

    initList();

    dlg = new TilesDlg(0, view->getScene());
    connect(dlg, SIGNAL(setLayerVisible(int)), view, SLOT(setLayerVisible(int)));
//    dlg->exec();


    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(view);
/*    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->setContentsMargins(20, 20, 20, 50);
    pathLayout->setSpacing(0);
    pathLayout->addWidget(pathList);
    layout->addLayout(pathLayout);
    layout->setStretchFactor(map, 3);
    layout->setStretchFactor(pathLayout, 1);
*/    setLayout(layout);

    connect(view, SIGNAL(transferData(QVector<long>&, QVector<long>&, QStringList&)),
            this, SIGNAL(transferData(QVector<long>&, QVector<long>&, QStringList&)));

}

void MapWidget::initList()
{
}

void MapWidget::addPath(DeliveryPath *path)
{
    paths.append(path);
    view->drawPath(path);
    QListWidgetItem *item = new QListWidgetItem;/*
    QFont font;
    font.setPixelSize(15);
    item->setFont(font);*/
    item->setText(path->m_status);
    pathList->addItem(item);
}

void MapWidget::removePath(int index)
{
    paths.takeAt(index);
    view->removePath(index);
    pathList->takeItem(index);
}

void MapWidget::updatePath(int index, DeliveryPath *path)
{
    paths[index] = path;
    pathList->item(index)->setText(path->m_status);
    view->updatePath(index, path);
}
