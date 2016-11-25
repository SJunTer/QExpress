#include "mapview.h"
#include "maplayer.h"
#include "miffile.h"
#include "gstfile.h"
#include "pixmapitem.h"
#include "deliverywidget.h"
#include <QDir>
#include <QPalette>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QPixmap>
#include <QBoxLayout>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <algorithm>
#include <QDebug>

MapView::MapView(QWidget *parent = 0)
    : QGraphicsView(parent)
    , zoom(0.0)
    , zoomLevel(18)
    , originLevel(zoomLevel)
    , repoSelected(false)
    , selectMode(false)
{

//    setStyleSheet("background: transparent");

    graph = new Graph;
    mifFile = new MifFile;
    gstFile = new GstFile;
    scene = new QGraphicsScene;

    // set widget attributes
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::ScrollHandDrag); //手型拖动
    setScene(scene);

    // read gst file for definition of layers
    QFileInfoList fileList = getFileList("gst");
    if(!fileList.empty())
        initLayers(fileList.at(0).filePath());
    // get definition of each layer
    getLayerDefn();
    centerOn(center);

    // store as algorithm style
    convertData();
    graph->GenerateGraph(vertexs.size(), distances.size(), distances);

    qDebug() << scene->itemsBoundingRect();
}

MapView::~MapView()
{
    delete gstFile;
    delete mifFile;
    delete graph;
    for(int i = 0; i < layers.size(); ++i)
    {
        MapLayer *l = layers.takeFirst();
        delete l;
    }
}


void MapView::initLayers(QString filePath)
{
    gstFile->openFile(filePath.toStdString().c_str());
    gstFile->getGeoDefn(originCenter, zoom);
    center = originCenter;
    qDebug() << center;
    //-------------设置初始缩放等级
    zoom = levelToZoom(zoomLevel);
    gstFile->getTableDefn(layers);
    gstFile->closeFile();
}

void MapView::getLayerDefn()
{
    // reversely add layers to sene
    for(int i = layers.size()-1; i >= 0; --i)
    {
        QString path = QString("./maps/%1.mif").arg(layers.at(i)->getName());
        mifFile->openFile(path.toStdString().c_str());
        mifFile->getLayerDefn(layers.at(i));
        layers.at(i)->addToScene(scene);
        layers.at(i)->setVisible(zoom);
        mifFile->closeFile();
    }
}

void MapView::layerVisible(double zoom)
{
    for(auto &layer : layers)
        layer->setVisible(zoom);
}

double MapView::levelToZoom(int level)
{
    return pow(2, (MAX_LEVEL-level))*0.75;
}

int MapView::zoomToLevel(double zoom)
{
    return MAX_LEVEL - log2(zoom);
}


/*************************************
 **                     数据转化                            **
 *************************************/

void MapView::convertData()
{
    if(layers.empty())
        return;
    for(auto &layer : layers)// 遍历每个图层
    {
        for(auto line : layer->m_lines)//遍历图层中的每条路线
        {
            for(auto point : line->getPoints())// 遍历每条线上的点
            {
                //生成点集
                vertexs.append(point);
            }
        }
    }
    //重排点集
    sortPoint(vertexs);
    for(auto &layer : layers)// 遍历每个图层
    {
        for(auto line : layer->m_lines)//遍历图层中的每条路线
        {
            auto points = line->getPoints();
            for(auto iter = points.begin(); iter != points.end()-1; ++iter)// 遍历每条线上的点
            {
                dist d;
                d.vertexA = binarySearch(vertexs, *iter, 0, vertexs.size()-1);
                d.vertexB = binarySearch(vertexs, *(iter+1), 0, vertexs.size()-1);
                d.length = getLen(*iter, *(iter+1));
                distances.append(d);

                long temp = d.vertexA;
                d.vertexA = d.vertexB;
                d.vertexB = temp;
                distances.append(d);
            }
        }
    }
}

// 二分搜索点集
long MapView::binarySearch(QVector<QPointF> &points, QPointF point, int xx, int yy)
{
    int m = xx + (yy - xx) / 2;
    if(xx > yy)
        return -1;
    else
    {
        if(points[m].x() == point.x())
        {
            if(points[m].y() == point.y())
                return m;
            else if(points[m].y() > point.y())
                return binarySearch(points, point, xx, m - 1);
            else
                return binarySearch(points, point, m + 1, yy);
        }
        else if(points[m].x() > point.x())
            return binarySearch(points, point, xx, m - 1);
        else
            return binarySearch(points, point, m + 1, yy);
    }
}

// 将点集排序并消除重复项
void MapView::sortPoint(QVector<QPointF> &theVector)
{
    struct cmp
    {
        bool operator() (const QPointF a, const QPointF b)
        {
            if(a.x() == b.x())
                return a.y() > b.y();
            else
                return a.x() > b.x();
        }
    };
    priority_queue<QPointF, QVector<QPointF>, cmp> myQueue;
    QVector <QPointF>::iterator Iter1;
    for (Iter1 = theVector.begin(); Iter1 != theVector.end(); ++Iter1)
    {
        myQueue.push(*Iter1);
    }

    QVector<QPointF>().swap(theVector);

    theVector.push_back(myQueue.top());
    myQueue.pop();

    while(!myQueue.empty())
    {
        if (myQueue.top() == theVector.back());
        else
        {
            theVector.push_back(myQueue.top());
        }
        myQueue.pop();
    }
}

double MapView::getLen(QPointF &p1, QPointF &p2)
{
    return sqrt(pow((p1.x() - p2.x()), 2) + pow((p1.y() - p2.y()), 2));
}


/******************************************
 **                             标记部分                            **
 ******************************************/

void MapView::addMarker(QPointF p, int type)
{
    PixmapItem *mark = new PixmapItem;
    if(type == MARK_REPO)
    {
        mark->setPixmap(QPixmap(":/images/repo_marker_48.png"));
        mark->setOffset(p.x()-ICON_SIZE/2+1, p.y()-ICON_SIZE);
        mark->setAlignPos(PixmapItem::BottomCenter);
        markers.push_front(mark);
    }
    else if(type ==MARK_POINT)
    {
        mark->setPixmap(QPixmap(":/images/map_marker_48.png"));
        mark->setOffset(p.x()-ICON_SIZE/2+1, p.y()-ICON_SIZE);
        mark->setAlignPos(PixmapItem::BottomCenter);
        markers.push_back(mark);
    }
    scene->addItem(mark);
    scene->update();
}

void MapView::removeMarker(int index)
{
    scene->removeItem(markers.at(index));
    scene->update();
    PixmapItem *p = markers.takeAt(index);
    delete p;
}

void MapView::clearMarker()
{
    for(auto n : markers)
    {
        scene->removeItem(markers[0]);
        PixmapItem *p = markers.takeFirst();
        delete p;
    }
    scene->update();
}


/*******************************************
 **                             路径相关                                           **
 *******************************************/

void MapView::drawPath(DeliveryPath *path)
{
    PathMarker pathMarker;
    pathMarker.id = path->id;
    pathMarker.pos = 0;

    // 路线
    Polyline *line = new Polyline;
    for(auto p : path->places)
        line->addPoint(p.coord.x(), p.coord.y());

    line->setBounds();
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(5);
    pen.setCosmetic(true);
    line->setPen(pen);
    pathMarker.lines.append(line);
    scene->addItem(line);

    // 配送点标记
    for(int i = 0; i < path->places.size(); ++i)
    {
        QPointF p = path->places[i].coord;
        if(path->places[i].type == IsRepo)
        {
            PixmapItem *mark = new PixmapItem;
            mark->setPixmap(QPixmap(":/images/repo_marker_48.png"));
            mark->setOffset(p.x()-ICON_SIZE/2+1, p.y()-ICON_SIZE);
            mark->setAlignPos(PixmapItem::BottomCenter);
            pathMarker.marker.append(mark);
            scene->addItem(mark);
        }
        else if(path->places[i].type == IsDely)
        {
            PixmapItem *mark = new PixmapItem;
            mark->setPixmap(QPixmap(":/images/map_marker_48.png"));
            mark->setOffset(p.x()-ICON_SIZE/2+1, p.y()-ICON_SIZE);
            mark->setAlignPos(PixmapItem::BottomCenter);
            pathMarker.marker.append(mark);
            scene->addItem(mark);
        }
    }

    // 车辆标记
    PixmapItem *car = new PixmapItem;
    car->setZValue(2);
    car->setPixmap(QPixmap(":/images/truck_icon_48.png"));
    QPointF p = path->places[0].coord;
    car->setOffset(p.x()-ICON_SIZE/2, p.y()-ICON_SIZE/2);
    car->setAlignPos(PixmapItem::Center);
    pathMarker.car = car;
    scene->addItem(car);

    scene->update();
    paths.append(pathMarker);
}

void MapView::updatePath(DeliveryPath *path)
{
    for(int i = 0; i < paths.size(); ++i)
    {
        if(paths[i].id == path->id)
        {
            for(int j = paths[i].pos; j < path->pos; ++j)
            {
                Polyline *line = new Polyline;
                line->addPoint(path->places[j].coord.x(), path->places[j].coord.y());
                line->addPoint(path->places[j+1].coord.x(), path->places[j+1].coord.y());
                line->setBounds();
                QPen pen;
                pen.setColor(Qt::blue);
                pen.setWidth(5);
                pen.setCosmetic(true);
                line->setPen(pen);
                paths[i].lines.append(line);
                scene->addItem(line);
            }
            paths[i].pos = path->pos;

            QPointF p = path->places[path->pos].coord;
            paths[i].car->setOffset(p.x()-ICON_SIZE/2, p.y()-ICON_SIZE/2);
            scene->update();
            break;
        }
    }
}

void MapView::removePath(int id)
{
    for(int i = 0; i < paths.size(); ++i)
    {
        if(paths[i].id == id)
        {
            for(auto n : paths[i].lines)
            {
                scene->removeItem(paths[i].lines.front());
                Polyline *l = paths[i].lines.takeFirst();
                delete l;
            }

            for(auto n : paths[i].marker)
            {
                scene->removeItem(paths[i].marker.front());
                PixmapItem *l = paths[i].marker.takeFirst();
                delete l;
            }

            scene->removeItem(paths[i].car);
            delete paths[i].car;

            paths.takeAt(i);
            break;
        }
    }
}


//返回矩形范围内的可见标记
QList<Point *> MapView::getVsibleSymbols(QRectF rect, int level)
{
    QList<Point *> points;
    double zm = levelToZoom(level);
    for(int i = 0; i < layers.size(); ++i)
    {
        if(layers[i]->isVisible(zm))    // 获得可见图层
        {
            QList<Point *> ps = layers[i]->m_points;
            for(int j = 0; j < ps.size(); ++j)  // 遍历标记
            {
                if(rect.contains(ps[j]->points[0])) // 如果在矩形内
                    points.push_back(ps[j]);
            }
        }
    }
    return points;
}




/***********************************************
 **                            重载部分                                      **
 ***********************************************/

void MapView::wheelEvent(QWheelEvent *event)
{
    center = mapToScene((event->pos()+rect().center())/2);
    // zoomLevel: 8~19
    if(event->delta() < 0)  //如果鼠标滚轮远离使用者，则delta()返回正值
        zoomOut();
    else
        zoomIn();
}


//重载菜单事件
void MapView::contextMenuEvent(QContextMenuEvent *event)
{
    if(!selectMode)
        return;

    // 判断item是否为point
    QList<QGraphicsItem*>itemList = items(event->pos());
    bool isPoint = false;
    for(int index = 0; index < itemList.size(); ++index)
    {
        Point *p = static_cast<Point*>(itemList[index]); // 强制转换
        if(p->featureClass == TABFCPoint)
        {
            tempPoint = p;
            isPoint = true;
            break;
        }
    }

    // 判断当前点是否在已选列表
    bool pointSelected = false;
    for(auto p : selectedPoints)
    {
        if(p == tempPoint) {
            pointSelected = true;
            break;
        }
    }

    QMenu menu;
    QAction *action1 = menu.addAction("标记为仓库");
    QAction *action2 = menu.addAction("标记为配送点");
    QAction *action3 = menu.addAction("删除标记");
    QAction *action4 = menu.addAction("确定选择");

    if(!isPoint) // 如果当前选中图形不是点
    {
        action1->setEnabled(false);
        action2->setEnabled(false);
        action3->setEnabled(false);
    }
    if(repoSelected) // 如果仓库已选
    {
        action1->setEnabled(false);
    }
    if(pointSelected) // 如果该点已选
    {
        action1->setEnabled(false);
        action2->setEnabled(false);
    }
    else//如果该点未选
    {
        action3->setEnabled(false);
    }

    connect(action1, SIGNAL(triggered(bool)), this, SLOT(addRepo()));
    connect(action2, SIGNAL(triggered(bool)), this, SLOT(addPlace()));
    connect(action3, SIGNAL(triggered(bool)), this, SLOT(removePoint()));
    connect(action4, SIGNAL(triggered(bool)), this, SLOT(calculatePath()));

    menu.exec(event->globalPos());
}


/*******************************************
 **                           响应菜单函数                          **
 *******************************************/

void MapView::addRepo()
{
    selectedPoints.push_front(tempPoint);
    repoSelected = true;
    addMarker(tempPoint->points[0], MARK_REPO);
}

void MapView::addPlace()
{
    selectedPoints.push_back(tempPoint);
    addMarker(tempPoint->points[0], MARK_POINT);
}

void MapView::removePoint()
{
    if(repoSelected && selectedPoints[0] == tempPoint) //移除仓库
    {
        selectedPoints.pop_front();
        repoSelected = false;
        removeMarker(0);
    }
    for(int i = 0; i < selectedPoints.size(); ++i)
    {
        if(selectedPoints[i] == tempPoint) {
            selectedPoints.takeAt(i);
            removeMarker(i);
            break;
        }
    }
}

// 计算路径
void MapView::calculatePath()
{
    if(!repoSelected) {
        QMessageBox::warning(this, tr("警告"), tr("请先选择仓库！"));
        return;
    }
    if(selectedPoints.size() < 2) {
        QMessageBox::warning(this, tr("警告"), tr("请至少选择一个配送点！"));
        return;
    }

    QVector<long> path, points;
    QList<Place> tempPlaces, places;
    qDebug() << "start selectedPoints";
    for(int index = 0; index < selectedPoints.size(); ++index)
    {
        Place p;
        p.title = selectedPoints[index]->getName();
        if(index == 0)
            p.type = IsRepo;
        else
            p.type = IsDely;
        p.coord = selectedPoints[index]->points[0];
        tempPlaces.push_back(p);

        //查找离标记点最近的路径点
        double min = 1000000000.0;
        long p_id = -1;
        for(int i = 0; i < vertexs.size(); ++i)
        {
            double len = getLen(vertexs[i], selectedPoints[index]->points[0]);
            if(len < min)
            {
                min = len;
                p_id = i;
            }
        }
        if(p_id == -1)  //  异常
            return;
        points.push_back(p_id);

        p.coord = vertexs[p_id];
        p.title = "";
        p.type = IsPass;
        tempPlaces.push_back(p);
    }
    qDebug() << "end selectedPoints";
    qDebug() << "start caculate";
    qDebug() << points.size();
    graph->BestPath(points, path);

    qDebug() << "end caculate";
    qDebug() << "start places";
    for(int i = 0; i < path.size(); ++i)    //path为返回路径点
    {
        Place p;
        int index = -1;
        for(int j = 0; j < tempPlaces.size(); ++j)
        {
            if(tempPlaces[j].coord == vertexs[path[i]])
            {
                index = j;
                break;
            }
        }
        if(index == -1) //  路径点不在配送点中
        {
            if(i == path.size() -1)   //终点
            {
                places.push_back(places[1]);
                places.push_back(places[0]);
            }
            else
            {
                p.coord = vertexs[path[i]];
                p.title = "";
                p.type = IsPass;
                places.push_back(p);
            }
        }
        else
        {
            if(i == 0)  // 起点
            {
                places.push_back(tempPlaces[index-1]);  //标记点
                places.push_back(tempPlaces[index]);    //路径点
                tempPlaces.takeAt(index);
                tempPlaces.takeAt(index-1);
            }
            else    //配送点
            {
                places.push_back(tempPlaces[index]);    //路径点
                places.push_back(tempPlaces[index-1]);  //标记点
                places.push_back(tempPlaces[index]);    //路径点
                tempPlaces.takeAt(index);
                tempPlaces.takeAt(index-1);
            }
        }
    }//while path
    qDebug() << "end places";

    //释放selectedPoints中所有元素
    selectedPoints.clear();
    clearMarker();
    repoSelected  = false;
    setSelectMode(false);

    //发送信号
    emit selectDone(places);

}


/**********************************
 **                  其他私有槽函数                **
 **********************************/
void MapView::zoomOut()
{
    if(zoomLevel > MIN_LEVEL) // 缩小视图
    {
        --zoomLevel;
        zoom = levelToZoom(zoomLevel);
        qDebug() << zoom;
        scale(0.5,0.5);
        layerVisible(zoom);
        centerOn(center);
        for(auto &n: markers)
            qDebug() << mapToScene(QPoint(n->boundingRect().center().x(), n->boundingRect().center().y()));
    }
}

void MapView::setLayerVisible(int level)
{
    if(level == 0)
        zoom = levelToZoom(zoomLevel);
    else
        zoom = levelToZoom(level);
    layerVisible(zoom);
}

void MapView::zoomIn()
{
    if(zoomLevel < MAX_LEVEL)
    {
        ++zoomLevel;
        zoom = levelToZoom(zoomLevel);
        qDebug() << zoom;
        scale(2,2);
        layerVisible(zoom);
        centerOn(center);
        for(auto &n: markers)
            qDebug() << mapToScene(QPoint(n->boundingRect().center().x(), n->boundingRect().center().y()));
    }
}


/******************************************
 **                           非成员函数                           **
 ******************************************/


QFileInfoList getFileList(QString filter)
{
    QDir dir("./maps/");
    // set suffix filter
    QStringList filters;
    filters << QString("*.")+filter;
    dir.setNameFilters(filters);
    QFileInfoList fileList(dir.entryInfoList(filters));
    return fileList;
}
