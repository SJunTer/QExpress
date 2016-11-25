#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QVector>
#include <QGraphicsView>
#include <QGraphicsItemGroup>
#include "deliverypath.h"
#include "../network/socket.h"

#define MIN_LEVEL 8
#define MAX_LEVEL 18

QT_BEGIN_NAMESPACE
class QThread;
class PixmapItem;
QT_END_NAMESPACE


class MapView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapView(QWidget *parent, ClientSocket *cli);
    ~MapView();

private:
    int zoomLevel;
    QPointF center;
    QRectF bound;
    ClientSocket *client;

    QGraphicsScene *scene;

    QVector<QString> tiles;
    QVector<QGraphicsItemGroup *> layers;
    QList<Place> places;
    QVector<QGraphicsItem *> path;
    PixmapItem *car;

    //*****控制多线程socket复用****//
    bool tileLoading;
    bool threadWaiting;
    QThread *thread;
    //************************//

    int getPreGeoInfo();
    bool preLoaded;
    void makeDir(QString &folderName);
    int getTiles();     //重新加载切片

protected:
    void wheelEvent(QWheelEvent *event); // 滚轮事件
    void mouseReleaseEvent(QMouseEvent *event);
//    void resizeEvent(QResizeEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);

signals:
    void stopLoading();

private slots:
    void setLoadFlag();

public slots:
    void zoomIn();      // 放大
    void zoomOut();     // 缩小
    void setLayerVisible();
    void drawPixmap(QString fileName, int lv, int x, int y);  //绘制切片(响应线程)
    void drawSymbol(QString symbolName, int lv,  int x, int y);  //绘制标记

    void drawPath(QList<Place> &ps);
    void updatePath(int pos);
    void cancelPath();

};

#endif // MAPWIDGET_H
