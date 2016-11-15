#include "mapshape.h"
#include "mitab.h"

#include <QPainter>
#include <QCursor>
#include <QPolygonF>
#include <QPainterPath>
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

    if(points.empty())
        return;

    painter->setRenderHint(QPainter::Antialiasing, true);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->drawPolyline(QPolygonF(points));
/*
    QPen outline = pen;
    outline.setWidthF(pen.widthF()+2.0);
    outline.setColor(Qt::black);
    painter->setPen(outline);
    painter->drawPolyline(QPolygonF(points));*/
    painter->setPen(pen);
    painter->drawPolyline(QPolygonF(points));

    /*
    QPainterPath path;
    path.moveTo(points[0]);
    for(int i = 1; i < points.size(); ++i)
        path.lineTo(points[i]);

    painter->setPen(Qt::black);
    painter->setBrush(pen.color());
    QPainterPathStroker stroker;
    stroker.setWidth(pen.width());
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);

    path = stroker.createStroke(path);
    painter->drawPath(path);*/

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

    painter->setRenderHint(QPainter::Antialiasing, true);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawPolygon(QPolygonF(points));
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

    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    pen.setCosmetic(true);
    painter->setPen(pen);
    if(autoLabel)
    {
        if(getName() != "")
        {
//            painter->drawEllipse(points[0], RADIUS, RADIUS);
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
