#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include <QMatrix>
#include <QPointF>
#include <QRect>
#include <QPixmap>
#include <QGraphicsItem>


class PixmapItem : public QGraphicsItem
{
public:
    enum AlignPos {
        TopLeft, Center, BottomCenter
    };
    PixmapItem();
    void setPixmap(const QPixmap &pix);
    void setOffset(qreal x, qreal y);
    void setAlignPos(AlignPos p);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    qreal top, left;
    qreal width, height;
    QPixmap pixmap;
    AlignPos pos;
};

// 返回缩放时固定尺寸的坐标矩阵
QMatrix stableMatrix(const QMatrix &matrix, const QPointF &p);


#endif // PIXMAPITEM_H
