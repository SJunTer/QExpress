#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QVector>
#include <QFileInfo>
#include <QList>
#include <QtMath>
#include <QPushButton>
#include "maplayer.h"
#include "miffile.h"
#include "gstfile.h"
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

    void setSelectMode(bool m) {  selectMode = m;  }

    // 路径相关
    void drawPath(DeliveryPath *path);
    void removePath(int index);
    void updatePath(int index, DeliveryPath *path);

private:
    void initLayers(QString filePath);
    void getLayerDefn();

    QPointF center;
    GstFile *gstFile;
    MifFile *mifFile;
    QGraphicsScene *scene;
    QList<MapLayer *> layers;

    double zoom;
    int zoomLevel;
    void layerVisible(double zoom);
    void levelToZoom(int level);
    void zoomToLevel();

    double getLen(QPointF &p1, QPointF &p2);

    // 生成图论算法数据结构
    Graph *graph;
    QList<dist> distances;
    QVector<QPointF> vertexs;
    void convertData();
    void sortPoint(QVector<QPointF> &theVector);
    long binarySearch(QVector<QPointF> &points, QPointF point, int xx, int yy);

    void makeTile(QGraphicsScene *scene);

    QList<Polyline *> paths;
    QList<QGraphicsPixmapItem *> cars;

    // 储存标记图层
    QList<QGraphicsPixmapItem *> markers;
    void addMarker(QPointF p, int type);
    void removeMarker(int index);
    void clearMarker();

    // 储存标记点
    QList<Point *> selectedPoints;
    Point *tempPoint;
    bool repoSelected;

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

    void zoomIn(); // 放大
    void zoomOut(); // 缩小
};

QFileInfoList getFileList(QString filter); // 获取地图文件列表
void makeDir(const QString &folderName);

#endif // MAPVIEW_H
