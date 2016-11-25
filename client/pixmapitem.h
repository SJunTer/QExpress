#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include <QMatrix>
#include <QPointF>
#include <QRect>
#include <QPixmap>
#include <QGraphicsPixmapItem>

// 地图瓦片
class MapTileItem : public QGraphicsItem
{
public:
    MapTileItem();
    void setPixmap(const QPixmap &pix);
    void setOffset(qreal x, qreal y);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    qreal top, left;
    qreal width, height;
    QPixmap pixmap;
    QRectF rect;
};

// 标记
class SymbolItem : public QGraphicsItem
{
public:
    SymbolItem();
    void setCoord(qreal x1, qreal y1);
    void setPixmap(QPixmap &pix);
    void setTitile(QString &t);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    qreal x, y;
    qreal width, height;
    QPixmap pixmap;
    QString title;
};


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
