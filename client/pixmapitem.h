#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include <QMatrix>
#include <QPointF>
#include <QRect>
#include <QPixmap>
#include <QGraphicsPixmapItem>


class PixmapItem : public QGraphicsItem
{
public:
    PixmapItem();
    void setPixmap(const QPixmap &pix);
    void setOffset(qreal x, qreal y);

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    qreal top, left;
    qreal width, height;
    QPixmap pixmap;

};

// 返回缩放时固定尺寸的坐标矩阵
QMatrix stableMatrix(const QMatrix &matrix, const QPointF &p);


#endif // PIXMAPITEM_H
