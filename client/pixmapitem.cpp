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
}

void MapTileItem::setPixmap(const QPixmap &pix)
{
    pixmap = pix;
    width = pix.width();
    height = pix.height();
    rect.setWidth(width);
    rect.setHeight(height);
}

void MapTileItem::setOffset(qreal x, qreal y)
{
    left = x;
    top = y;
    rect.setX(left);
    rect.setY(top);
}

QRectF MapTileItem::boundingRect() const
{
 //    return QRectF(left, top, width, height);
   return rect;
}

void MapTileItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QMatrix oldMatrix = painter->worldMatrix();
    painter->setMatrix(stableMatrix(oldMatrix, QPointF(left, top)));
    painter->drawPixmap(left, top, width, height, pixmap);
    rect.setWidth(width*(1.0/oldMatrix.m11()));
//    rect.setWidth();
    rect.setHeight(height*(1.0/oldMatrix.m22()));
//    painter->drawRect(boundingRect());
//    qDebug() << rect;
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
//    qDebug("%10f\t%10f", p.x(), p.y());
//    qDebug("%10f\t%10f\n", offsetX, offsetY);
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
    rect.setX(x1);
    rect.setY(y1);
}

void SymbolItem::setPixmap(QPixmap &pix)
{
    pixmap = pix;
    width = pixmap.width();
    height = pixmap.height();
    rect.setWidth(width);
    rect.setHeight(height);
}

void SymbolItem::setTitile(QString &t)
{
    title = t;
}

QRectF SymbolItem::boundingRect() const
{
//    return QRectF(x-width/2, y-height/2, width, height);
    return QRectF(rect.x()-rect.width()/2.0, rect.y()-rect.height()/2.0, rect.width(), rect.height());
}

void SymbolItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
     Q_UNUSED(widget);

    QMatrix oldMatrix = painter->worldMatrix();
     painter->setMatrix(stableMatrix(oldMatrix, QPointF(x, y)));
     rect.setWidth(width*(1.0/oldMatrix.m11()));
     rect.setHeight(height*(1.0/oldMatrix.m22()));

//     painter->drawEllipse(x, y, 10, 10);
     painter->drawPixmap(x-width/2, y-height/2, width, height, pixmap);
     QPen pen;
     pen.setCosmetic(true);
     painter->setPen(pen);
     QRectF rect;
     rect.setX(x+15);
     rect.setWidth(120);
     QString text = title;
     if(text.size() >= 8)
     {
         text.insert((text.size()%2?text.size()/2+1:text.size()/2), '\n');
         rect.setY(y-12);
         rect.setHeight(30);
     }
     else
     {
         rect.setY(y-5);
         rect.setHeight(15);
     }
     painter->drawText(rect, text);
}



/**************************************
 *                                 图标                                        *
 * ************************************/
PixmapItem::PixmapItem()
{
    top = left = width = height = 0.0;
    pos = TopLeft;
}

void PixmapItem::setPixmap(const QPixmap &pix)
{
    pixmap = pix;
    width = pix.width();
    height = pix.height();
}

void PixmapItem::setOffset(qreal x, qreal y)
{
    left = x;
    top = y;
}

void PixmapItem::setAlignPos(PixmapItem::AlignPos p)
{
    pos = p;
}


QRectF PixmapItem::boundingRect() const
{
    return QRectF(left, top, width, height);
}

void PixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPointF p;
    switch(pos)
    {
    case TopLeft: p = QPointF(left, top); break;
    case Center: p = QPointF(left+width/2-2, top+height/2-2); break;
    case BottomCenter: p = QPointF(left+width/2-2, top+height); break;
    default: break;
    }
    painter->setMatrix(stableMatrix(painter->worldMatrix(), p));
    painter->drawPixmap(left, top, width, height, pixmap);

}
