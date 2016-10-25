#include "maplayer.h"
#include <QDebug>
#include <cmath>

MapLayer::MapLayer()
{}

void MapLayer::addShape(MapShape *shape, TABFeatureClass type)
{
    switch(type)
    {
        case TABFCRegion:
            region.append((Region*)shape);
            break;
        case TABFCPolyline:
            lines.append((Polyline*)shape);
            break;
        case TABFCPoint:
            point.append((Point*)shape);
            break;
    }
}

void MapLayer::drawLayer(QGraphicsScene *scene)
{
    for(auto r : region)
        scene->addItem(r);
    for(auto l : lines)
        scene->addItem(l);
    for(auto p : point)
        scene->addItem(p);
}

void MapLayer::coordTransform(double &x, double &y)
{
    x = x / 360 * 256 * pow(2,18);
    y = -log(tan((90+y)/360*PI)) / (2*PI) * 256 * pow(2,18);
}

