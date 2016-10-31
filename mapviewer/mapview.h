#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QVector>
#include <QFileInfo>
#include <QList>
#include <QtMath>
#include <QPushButton>
#include "maplayer.h"
#include "mifreader.h"
#include "gstreader.h"
#include "optimumPath.h"

#define BTN_SIZE 25
#define MARK_REPO 1
#define MARK_POINT 2
#define ICON_SIZE 48

class DeliveryPath;

class MapView : public QGraphicsView
{
    Q_OBJECT
public:
    MapView(QWidget *parent);

    void setSelectMode(bool m){
        selectMode = m;
    }

    void drawPath(DeliveryPath *path); //绘制路径
    void removePath(int index);
    void updatePath(int index, DeliveryPath *path);

private:
    QFileInfoList getFileList(QString filter); // 获取地图文件列表
    void initLayers(QString filePath);
    void getLayerDefn();

    GstFile *gstFile;
    MifFile *mifFile;
    QGraphicsScene *scene;
    QList<MapLayer *> layers;
    double zoom;
    int zoomLevel;
    QPointF center;
    void layerVisible(double zoom);
    void levelToZoom();
    void zoomToLevel();

    // 生成图论算法数据结构
    void convertData();
    void generatePoint(QVector<QPointF> &theVector);
    long findPoint(QVector<QPointF> &points, QPointF point, int xx, int yy);
    double getLen(QPointF &p1, QPointF &p2);

    void saveFile(QList<dist> &distances);
    void readFile(QList<dist> &distances);

    QList<Polyline *> paths;

//    void makeTile(QGraphicsScene *scene);

    Graph *graph;

    //储存小车标记
    QList<QGraphicsPixmapItem *> cars;

    //储存标记图层
    QList<QGraphicsPixmapItem *> markers;
    void addMarker(QPointF p, int type);
    void removeMarker(int index);
    void clearMarker();

    // 储存标记点
    QList<Point *> selectedPoints;
    Point *tempPoint;
    bool repoSelected;

    //储存路径图层
    QList<QGraphicsItem *> pathLayer;

    double minX, minY, maxX, maxY;

    QList<dist> distances;
    QVector<QPointF> vertexs;

    bool selectMode;  // 标记当前是否为选择模式

    QPushButton *zoomInBtn; //放大按钮
    QPushButton *zoomOutBtn; //缩小按钮

protected:
    void wheelEvent(QWheelEvent *event); // 滚轮事件
    void contextMenuEvent(QContextMenuEvent *event); // 菜单事件

signals:
    void transferData(QVector<long> &points, QVector<long> &path, QStringList &nameList);

/* 私有槽函数，用来响应菜单 */
private slots:
    void addRepo(); // 添加仓库
    void addPlace(); // 添加配送点
    void removePoint(); // 移除标记点
    void calculatePath(); // 计算路径并返回配送界面
};

#endif // MAPVIEW_H
