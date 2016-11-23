#ifndef MAPSHAPE_H
#define MAPSHAPE_H

#include <QVector>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QPixmap>
#include <QGraphicsItem>
#include "mapfield.h"

#define POS_ICON_SIZE 10

class MapShape : public QGraphicsItem
{
public:
    MapShape();

    void setAutoLabel(bool b) { autoLabel = b; }
    void addPoint(double x, double y);
    void addField(const QString name, const QString type, const QString content);
    QString getName(void);
    virtual void setBounds() = 0;

protected:
    double minX, minY, maxX, maxY;
    QVector<QPointF> points;
    MapField fieldInfo;
    bool autoLabel;
};

class Polyline : public MapShape
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

class Region : public MapShape
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

class Point : public MapShape
{
    friend class MapView;
public:
    Point();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void setBounds(void);
    QPointF getPoint();
    int featureClass;

    void setPixmap(const QPixmap &pix);

private:
    qreal width, height;
    QPixmap pixmap;
};


#endif // MAPSHAPE_H
