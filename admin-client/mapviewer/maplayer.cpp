#include "maplayer.h"
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QDebug>
#include <cmath>

MapLayer::MapLayer()
    : name("")
    , minZoom(0.0)
    , maxZoom(1E10)
    , autoLabel(false)
//    , m_featureCnt(0)
{
    minX = minY = maxX = maxY = 0.0;
    group = new QGraphicsItemGroup;
}

void MapLayer::addItem(MapShape *shape, TABFeatureClass type)
{
    switch(type)
    {
    case TABFCRegion:
        m_regions.append((Region*)shape);
        break;
    case TABFCPolyline:
        m_lines.append((Polyline*)shape);
        break;
    case TABFCPoint:
        m_points.append((Point*)shape);
        break;
    default: break;
    }
}


void MapLayer::addToScene(QGraphicsScene *scene)
{
    for(auto &region : m_regions)
        group->addToGroup(region);
    for(auto &line : m_lines)
        group->addToGroup(line);
    for(auto &point : m_points)
        group->addToGroup(point);
    scene->addItem(group);
}

void MapLayer::setVisible(double zoom)
{
    if(zoom >= minZoom && zoom <= maxZoom)
        group->setVisible(true);
    else
        group->setVisible(false);
}


/*----------------------------------------------------------*/

void coordTransform(double &x, double &y)
{
    x = x / 360 * 256 * pow(2,18);
    y = -log(tan((90+y)/360*PI)) / (2*PI) * 256 * pow(2,18);
}
