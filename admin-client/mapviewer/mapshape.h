#ifndef MAPSHAPE_H
#define MAPSHAPE_H

#include <QVector>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include "mapfield.h"

#define RADIUS 10

class MapShape
{
public:
    MapShape();

    void setAutoLabel(bool b) { autoLabel = b; }
    void addPoint(double x, double y);
    void addField(const QString name, const QString type, const QString content);
    QString getName(void);
    virtual void setBounds() = 0;

    double minX, minY, maxX, maxY;
    QVector<QPointF> points;
    MapField fieldInfo;
    bool autoLabel;
};

class Polyline : public MapShape, public QGraphicsItem
{
public:
    Polyline();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    void setBounds(void);
    void setPen(const QPen &p) { pen = p; }
    QVector<QPointF> &getPoints(void);
    int featureClass;

private:
    QPen pen;
};

class Region : public MapShape, public QGraphicsItem
{
public:
    Region();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    void setBounds(void);
    void setPen(const QPen &p) { pen = p; }
    void setBrush(const QBrush &b) { brush = b; }
    int featureClass;

private:
    QPen pen;
    QBrush brush;
};

class Point : public MapShape, public QGraphicsItem
{
    friend class MapView;
public:
    Point();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void setBounds(void);
    int featureClass;

private:

};


#endif // MAPSHAPE_H
