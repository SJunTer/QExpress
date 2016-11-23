#ifndef DELIVERYPATH_H
#define DELIVERYPATH_H

#include <QTime>
#include <QPointF>
#include <QStringList>

//地点类型
enum PlaceType {
    IsRepo, IsDely, IsPass
};

// 途经点
struct Place
{
    QString title;  // 地名
    PlaceType type; //类型
    QPointF coord;    // 坐标
};

// 路径信息
struct DeliveryPath
{
    int truckId;
    QStringList cargos;
    QList<Place> places;
    int pos;    // 当前位置
    int mins;   // 运行时间
};


#endif // DELIVERYPATH_H
