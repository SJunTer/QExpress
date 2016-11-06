#include "mapshape.h"
#include "mitab.h"

#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>


/*------------------------MapShape---------------------*/
MapShape::MapShape()
    : minX(0.0)
    , minY(0.0)
    , maxX(0.0)
    , maxY(0.0)
{
}

void MapShape::addPoint(double x, double y)
{
    points.append(QPointF(x, y));
}

void MapShape::addField(const QString name, const QString type, const QString content)
{
    fieldInfo.addField(name, type, content);
}

QString MapShape::getName()
{
    return fieldInfo.getContent(fieldInfo.getFieldId("name"));
}


/*------------------------Polyline-------------------------*/
Polyline::Polyline()
    : featureClass(TABFCPolyline)
{
}

QRectF Polyline::boundingRect() const
{
    QRectF rect;
    int w = pen.width();
    rect.setTopLeft(QPointF(minX-w/2.0, minY-w/2.0));
    rect.setBottomRight(QPointF(maxX+w/2.0, maxY+w/2.0));
    return rect;
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
    /*
    if(autoLabel)
    {
        painter->setPen(Qt::black);
        painter->drawText(QPointF((minX+maxX)/2, (minY+maxY)/2), getName());
    }*/
}

void Polyline::setBounds(void)
{
    double xmin, xmax, ymin, ymax;
    xmin = xmax = points.front().x();
    ymin = ymax = points.front().y();
    for(auto &p : points)
    {
        if(p.x() < xmin)
            xmin = p.x();
        if(p.x() > xmax)
            xmax = p.x();
        if(p.y() < ymin)
            ymin = p.y();
        if(p.y() > ymax)
            ymax = p.y();
    }
    minX = xmin;
    minY = ymin;
    maxX = xmax;
    maxY = ymax;
}


QVector<QPointF> &Polyline::getPoints(void)
{
    return points;
}


/*------------------------Region-------------------------*/

Region::Region()
    : featureClass(TABFCRegion)
{
}

QRectF Region::boundingRect() const
{
    QRectF rect;
    int w = pen.width();
    rect.setTopLeft(QPointF(minX-w/2.0, minY-w/2.0));
    rect.setBottomRight(QPointF(maxX+w/2.0, maxY+w/2.0));
    return rect;
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
    /*
    if(autoLabel)
    {
        painter->setPen(Qt::black);
        painter->drawText(QPointF((minX+maxX)/2, (minY+maxY)/2), getName());
    }*/
}

void Region::setBounds(void)
{
    double xmin, xmax, ymin, ymax;
    xmin = xmax = points.front().x();
    ymin = ymax = points.front().y();
    for(auto p : points)
    {
        if(p.x() < xmin)
            xmin = p.x();
        if(p.x() > xmax)
            xmax = p.x();
        if(p.y() < ymin)
            ymin = p.y();
         if(p.y() > ymax)
            ymax = p.y();
    }
    minX = xmin;
    minY = ymin;
    maxX = xmax;
    maxY = ymax;

}


/*------------------------Point-------------------------*/

Point::Point()
    : featureClass(TABFCPoint)
{
    setCursor(Qt::PointingHandCursor);
}

QRectF Point::boundingRect() const
{
    return QRectF(minX, minY, maxX-minX, maxY-minY);
}

void Point::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen;
    pen.setCosmetic(true);
    painter->setPen(pen);
    if(autoLabel)
    {
        if(getName() != "")
        {
            painter->drawEllipse(points[0], RADIUS, RADIUS);
//            painter->drawText(QPointF(points[0].x(), points[0].y()),getName());
        }
    }
}

void Point::setBounds()
{
    minX = points[0].x() - RADIUS;
    maxX = points[0].x() + RADIUS;
    minY = points[0].y() - RADIUS;
    maxY = points[0].y() + RADIUS;
}
