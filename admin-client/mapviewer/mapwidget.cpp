#include "mapwidget.h"
#include "deliverypath.h"
#include <QBoxLayout>
#include <QListWidgetItem>
#include <QColor>
#include <QFont>
#include <QDebug>


MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent)
{
    map = new MapView(this);
    pathList = new QListWidget(this);

    initList();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(map);
    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->setContentsMargins(20, 20, 20, 50);
    pathLayout->setSpacing(0);
    pathLayout->addWidget(pathList);
    layout->addLayout(pathLayout);
    layout->setStretchFactor(map, 3);
    layout->setStretchFactor(pathLayout, 1);
    setLayout(layout);

    connect(map, SIGNAL(transferData(QVector<long>&, QVector<long>&, QStringList&)),
            this, SIGNAL(transferData(QVector<long>&, QVector<long>&, QStringList&)));

}

void MapWidget::initList()
{
}

void MapWidget::addPath(DeliveryPath *path)
{
    paths.append(path);
    map->drawPath(path);
    QListWidgetItem *item = new QListWidgetItem;/*
    QFont font;
    font.setPixelSize(15);
    item->setFont(font);*/
    item->setText(path->status);
    pathList->addItem(item);
}

void MapWidget::removePath(int index)
{
    paths.takeAt(index);
    map->removePath(index);
    pathList->takeItem(index);
}

void MapWidget::updatePath(int index, DeliveryPath *path)
{
    paths[index] = path;
    pathList->item(index)->setText(path->status);
    map->updatePath(index, path);
}
