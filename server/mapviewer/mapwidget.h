#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QList>
#include <QCheckBox>
#include <QStringList>
#include <QVector>
#include <QListWidget>
#include "mapviewer/mapview.h"

class DeliveryPath;


///////////////////////////
/// The MapWidget class ////
//////////////////////////
class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = 0);
    void setSelectMode(bool b)
    { map->setSelectMode(true); }

private:
    void initList();

    MapView *map;
    QListWidget *pathList;
    QList<DeliveryPath *> paths;

signals:
    void transferData(QVector<long> &points, QVector<long> &path, QStringList &nameList);

public slots:
    void addPath(DeliveryPath *path);
    void removePath(int index);
    void updatePath(int index, DeliveryPath *path);
};

#endif // MAPWIDGET_H
