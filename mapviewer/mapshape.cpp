#include "mapshape.h"
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

Polyline::Polyline()
    : featureClass(TABFCPolyline)
{
}

QRectF Polyline::boundingRect() const
{
    return QRectF(minX, minY, maxX-minX, maxY-minY);
}

void Polyline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    pen.setCosmetic(true);
    painter->setPen(pen);
    for(auto iter = points.begin(); iter != points.end()-1; ++iter)
    {
        painter->drawLine(*iter, *(iter+1));
    }
    painter->setPen(Qt::black);
//    painter->drawText(QPointF((minX+maxX)/2, (minY+maxY)/2),
 //                     fieldInfo.getContent(fieldInfo.getFieldId("name")));
}

void Polyline::addPoint(int x, int y)
{
    points.append(QPointF(x, y));
}

void Polyline::setBounds()
{
    double xmin, xmax, ymin, ymax;
    xmin = xmax = points.front().x();
    ymin = ymax = points.front().y();
    for(auto p : points)
    {
        if(p.x() < xmin)
            xmin = p.x();
        else if(p.x() > xmax)
            xmax = p.x();
        else if(p.y() < ymin)
            ymin = p.y();
        else if(p.y() > ymax)
            ymax = p.y();
    }
    minX = xmin;
    minY = ymin;
    maxX = xmax;
    maxY = ymax;

}

void Polyline::addField(const QString name, const QString type, const QString content)
{
    fieldInfo.addField(name, type, content);
}

QVector<QPointF> &Polyline::getPoints()
{
    return points;
}

Region::Region()
    : featureClass(TABFCRegion)
{

}

QRectF Region::boundingRect() const
{
    return QRectF(minX, minY, maxX-minX, maxY-minY);
}

void Region::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
     Q_UNUSED(widget);

    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(brush);

    int size = points.size();
    QPointF *vertexs = new QPointF[size];
    for(int i = 0; i < size; ++i)
        vertexs[i] = points.at(i);
    painter->drawPolygon(vertexs, size);
    painter->setPen(Qt::black);
//    painter->drawText(QPointF((minX+maxX)/2, (minY+maxY)/2),
//                      fieldInfo.getContent(fieldInfo.getFieldId("name")));
}

void Region::addPoint(int x, int y)
{
    points.append(QPointF(x, y));
}

void Region::setBounds()
{
    double xmin, xmax, ymin, ymax;
    xmin = xmax = points.front().x();
    ymin = ymax = points.front().y();
    for(auto p : points)
    {
        if(p.x() < xmin)
            xmin = p.x();
        else if(p.x() > xmax)
            xmax = p.x();
        else if(p.y() < ymin)
            ymin = p.y();
        else if(p.y() > ymax)
            ymax = p.y();
    }
    minX = xmin;
    minY = ymin;
    maxX = xmax;
    maxY = ymax;

}

void Region::addField(const QString name, const QString type, const QString content)
{
    fieldInfo.addField(name, type, content);
}

Point::Point()
    : featureClass(TABFCPoint)
{
    setCursor(Qt::PointingHandCursor);
}

QRectF Point::boundingRect() const
{
    return QRectF(point.x()-RADIUS, point.y()-RADIUS, 2*RADIUS, 2*RADIUS);
}

void Point::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPen pen;
    pen.setCosmetic(true);
    painter->setPen(pen);
    if(getName() != "")
    {
        painter->drawEllipse(point, RADIUS, RADIUS);
        painter->drawText(QPointF(point.x(), point.y()),getName());
    }
}

void Point::addPoint(int x, int y)
{
    point.setX(x);
    point.setY(y);
}

void Point::addField(const QString name, const QString type, const QString content)
{
    fieldInfo.addField(name, type, content);
}

//返回name字段
QString Point::getName()
{
    return fieldInfo.getContent(fieldInfo.getFieldId("name"));
}

MapShape::MapShape()
{
}
