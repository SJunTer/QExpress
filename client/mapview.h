#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include "../network/socket.h"

#define BTN_SIZE 25
#define MIN_LEVEL 8
#define MAX_LEVEL 18

class MapView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapView(QWidget *parent, ClientSocket *cli);

private:
    int zoomLevel;
    QPointF center;
    QRectF bound;
    ClientSocket *client;

    QGraphicsPixmapItem *item;
    QGraphicsScene *scene;
    QPushButton *zoomInBtn;
    QPushButton *zoomOutBtn;

    int getPreGeoInfo();
    bool preLoaded;
    void makeDir(QString &folderName);
    bool isExisted(QString &fileName);

protected:
    void wheelEvent(QWheelEvent *event); // 滚轮事件
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void zoomIn();      // 放大
    void zoomOut();     // 缩小
    int getTiles();     //重新加载切片
    int getSymbols();   // 获得标记信息

};

#endif // MAPWIDGET_H
