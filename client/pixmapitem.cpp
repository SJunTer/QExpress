#include "pixmapitem.h"
#include <QPainter>


PixmapItem::PixmapItem()
{
    top = left = width = height = 0.0;
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


QRectF PixmapItem::boundingRect() const
{
    return QRectF(left, top, width, height);
}

void PixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPointF p(left, top);
    painter->setMatrix(stableMatrix(painter->worldMatrix(), p));
    painter->drawPixmap(left, top, width, height, pixmap);
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
