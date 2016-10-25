#ifndef MAPLAYER_H
#define MAPLAYER_H

#include <QVector>
#include <QGraphicsScene>
#include "mapshape.h"

class MapLayer
{
    friend class MapView;
public:
    MapLayer();
    void addShape(MapShape *shape, TABFeatureClass type);
    void drawLayer(QGraphicsScene *scene);
    void getFeatureCount(int cnt) { featureCount = cnt; }
    void coordTransform(double &x, double &y);

private:
    QVector<Polyline *> lines;
    QVector<Region *> region;
    QVector<Point *> point;
    int featureCount;
    double minX, minY, maxX, maxY;
};

#endif // MAPLAYER_H
