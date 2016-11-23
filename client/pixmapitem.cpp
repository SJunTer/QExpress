#include "pixmapitem.h"
#include <QPainter>
#include <QPixmap>
#include <QCursor>
#include <QDebug>



/**************************************
 *                             瓦片图形                                     *
 * ************************************/
MapTileItem::MapTileItem()
{
    top = left = width = height = 0.0;
    setCursor(Qt::UpArrowCursor);
}

void MapTileItem::setPixmap(const QPixmap &pix)
{
    pixmap = pix;
    width = pix.width();
    height = pix.height();
}

void MapTileItem::setOffset(qreal x, qreal y)
{
    left = x;
    top = y;
}

QRectF MapTileItem::boundingRect() const
{
    return QRectF(left, top, width, height);
 //   return rect;
}

void MapTileItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QMatrix oldMatrix = painter->worldMatrix();
    painter->setMatrix(stableMatrix(oldMatrix, QPointF(left, top)));
    painter->drawPixmap(left, top, width, height, pixmap);
    painter->drawRect(boundingRect());
}


QMatrix stableMatrix(const QMatrix &matrix, const QPointF &p)
{
    QMatrix newMatrix = matrix;

    qreal scaleX, scaleY;
    scaleX = newMatrix.m11();
    scaleY = newMatrix.m22();
    newMatrix.scale(1.0/scaleX, 1.0/scaleY);

    qreal offsetX, offsetY;
    offsetX = (p.x())*(scaleX-1.0);
    offsetY = (p.y())*(scaleY-1.0);
    qDebug("%10f\t%10f", p.x(), p.y());
    qDebug("%10f\t%10f\n", offsetX, offsetY);
    newMatrix.translate(offsetX, offsetY);
//     newMatrix.translate(-p.x(), -p.y());

    return newMatrix;
}


/**************************************
 *                             标记图形                                     *
 * ************************************/

SymbolItem::SymbolItem()
{
    x = y = width = height = 0.0;
    setCursor(Qt::PointingHandCursor);
}

void SymbolItem::setCoord(qreal x1, qreal y1)
{
    x = x1;
    y = y1;
}

void SymbolItem::setPixmap(QPixmap &pix)
{
    pixmap = pix;
    width = pixmap.width();
    height = pixmap.height();
}

void SymbolItem::setTitile(QString &t)
{
    title = t;
}

QRectF SymbolItem::boundingRect() const
{
    return QRectF(x-width/2, y-height/2, width, height);
}

void SymbolItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
     Q_UNUSED(widget);

     painter->setRenderHint(QPainter::Antialiasing, true);
     painter->setMatrix(stableMatrix(painter->worldMatrix(), QPointF(x, y)));

     painter->drawEllipse(x, y, 10, 10);
//     painter->drawPixmap(x-width/2, y-height/2, width, height, pixmap);
     /*     QPen pen;
              pen.setCosmetic(true);
              painter->setPen(pen);
             painter->drawText(QPointF(points[0].x(), points[0].y()),getName());*/
}
