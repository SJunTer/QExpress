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

MapLayer::~MapLayer()
{
    for(int i = 0; i < m_regions.size(); ++i)
    {
        Region *r = m_regions.takeFirst();
        delete r;
    }
    for(int i = 0; i < m_lines.size(); ++i)
    {
        Polyline *l = m_lines.takeFirst();
        delete l;
    }
    for(int i = 0; i < m_points.size(); ++i)
    {
        Point *p = m_points.takeFirst();
        delete p;
    }
    if(group)
        delete group;
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
    group->setVisible(isVisible(zoom));
}

bool MapLayer::isVisible(double zoom)
{
    return (zoom >= minZoom && zoom <= maxZoom);
}


// 墨卡托坐标和经纬度转换
void coordTransform(double &x, double &y)
{
    x = x / 360 * 256 * pow(2,18);
    y = -log(tan((90+y)/360*PI)) / (2*PI) * 256 * pow(2,18);

/*    x = (x-116.0) / 360 * 256 * pow(2,18);
    y = -(y-28.6) / 360 * 256 * pow(2,18);*/
}
