#ifndef MAPLAYER_H
#define MAPLAYER_H

#include <QList>
#include <QString>
#include <QBrush>
#include <QPen>
#include "mapshape.h"
#include "mitab.h"

class QGraphicsScene;
class QGraphicsItemGroup;

class MapLayer
{
    friend class MapView;
    friend class GstFile;
    friend class MifFile;
public:
    MapLayer();
    ~MapLayer();

    void addItem(MapShape *shape, TABFeatureClass type);
    void addToScene(QGraphicsScene *scene);
    void setVisible(double zoom);
    void setBrush(QBrush &brush) { m_brush = brush; }
    void setPen(QPen &pen) { m_pen = pen; }
    QString getName() { return name; }
    bool isVisible(double zoom);
//    void setFeatureCnt(int cnt) { m_featureCnt = cnt;  }

private:
    QString name;
    double minZoom, maxZoom;
    // Override style
    QBrush m_brush;
    QPen m_pen;
    bool autoLabel;

    QGraphicsItemGroup *group;
    QList<Polyline *> m_lines;
    QList<Region *> m_regions;
    QList<Point *> m_points;

//    int m_featureCnt;
    double minX, minY, maxX, maxY;
};

void coordTransform(double &x, double &y);


#endif // MAPLAYER_H
