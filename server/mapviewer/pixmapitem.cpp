#include "pixmapitem.h"
#include <QPainter>
#include <QDebug>


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
//    qDebug("%.10f, %.10f", boundingRect().x(), boundingRect().y());
//    painter->drawRect(boundingRect());
}


QMatrix stableMatrix(const QMatrix &matrix, const QPointF &p)
{
    QMatrix newMatrix = matrix;

    qreal scaleX, scaleY;
    scaleX = newMatrix.m11();
    scaleY = newMatrix.m22();
    newMatrix.scale(1.0/scaleX, 1.0/scaleY);

    qreal offsetX, offsetY;
    offsetX = p.x()*(scaleX-1.0);
    offsetY = p.y()*(scaleY-1.0);
    newMatrix.translate(offsetX, offsetY);

    return newMatrix;
}
