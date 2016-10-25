#ifndef MAPSHAPE_H
#define MAPSHAPE_H

#include <QVector>
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QMouseEvent>
#include <QPointF>
#include "mitab.h"
#include "mapfield.h"

#define RADIUS 10


class MapShape
{
public:
    MapShape();
    virtual void addPoint(int x, int y) = 0;
    virtual void setIndex(int i) = 0;
    virtual int getIndex() = 0;
    virtual void setBounds() = 0;
    virtual void setPen(const QPen&) = 0;
    virtual void setBrush(const QBrush&) = 0;
    virtual void addField(const QString name, const QString type, const QString content) = 0;


};

class Polyline : public MapShape, public QGraphicsItem
{
public:
    Polyline();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    virtual void addPoint(int x, int y);
    virtual void setIndex(int i) { index = i; }
    virtual int getIndex() { return index; }
    virtual void setBounds();
    virtual void setPen(const QPen &p) { pen = p; }
    virtual void setBrush(const QBrush &b) {}
    virtual void addField(const QString name, const QString type, const QString content);
    QVector<QPointF> &getPoints();

    int featureClass;

private:
    QVector<QPointF> points;
    int index;
    double minX, minY, maxX, maxY;
    QPen pen;
    MapField fieldInfo;
};

class Region : public MapShape, public QGraphicsItem
{
public:
    Region();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    virtual void addPoint(int x, int y);
    virtual void setIndex(int i) { index = i; }
    virtual int getIndex() { return index; }
    virtual void setBounds();
    virtual void setPen(const QPen &p) { pen = p; }
    virtual void setBrush(const QBrush &b) { brush = b; }
    virtual void addField(const QString name, const QString type, const QString content);

    int featureClass;

private:
    QVector<QPointF> points;
    int index;
    double minX, minY, maxX, maxY;
    QPen pen;
    QBrush brush;
    MapField fieldInfo;
};

class Point : public MapShape, public QGraphicsItem
{
    friend class MapView;
public:
    Point();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    virtual void addPoint(int x, int y);
    virtual void setIndex(int i) { index = i; }
    virtual int getIndex() { return index; }
    virtual void setBounds(){}
    virtual void setPen(const QPen &p) {}
    void setBrush(const QBrush &b) {}
    virtual void addField(const QString name, const QString type, const QString content);
    QString getName();

    int featureClass;

private:
    QPointF point;
    int index;
    MapField fieldInfo;

};

#endif // MAPSHAPE_H
