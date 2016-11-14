#include "mapview.h"
#include "deliverypath.h"
#include <QDebug>
#include <QDir>
#include <QPalette>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QWheelEvent>
#include <QTransform>
#include <QContextMenuEvent>
#include <QBoxLayout>
#include <algorithm>
//#include <cstdio>

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

    zoomInBtn = new QPushButton(this);
    zoomOutBtn = new QPushButton(this);

    QFont font;
    font.setPixelSize(15);
    zoomInBtn->setFont(font);
    zoomOutBtn->setFont(font);
    zoomInBtn->setText("+");
    zoomOutBtn->setText("-");
    zoomInBtn->setFixedSize(BTN_SIZE,BTN_SIZE);
    zoomOutBtn->setFixedSize(BTN_SIZE,BTN_SIZE);

    QVBoxLayout *btnLayout = new QVBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setMargin(0);
    btnLayout->addWidget(zoomInBtn);
    btnLayout->addWidget(zoomOutBtn);
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(0);
    gridLayout->setMargin(10);
    gridLayout->addLayout(btnLayout, 1, 1, Qt::AlignRight|Qt::AlignBottom);
    setLayout(gridLayout);

    connect(zoomInBtn, SIGNAL(clicked(bool)), this, SLOT(zoomIn()));
    connect(zoomOutBtn, SIGNAL(clicked(bool)), this, SLOT(zoomOut()));

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
//    convertData();
//    graph->GenerateGraph(vertexs.size(), distances.size(), distances);


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
    //-------------设置初始缩放等级
    levelToZoom(zoomLevel);
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

void MapView::levelToZoom(int level)
{
    zoom = pow(2, (MAX_LEVEL-level))*0.75;
}

void MapView::zoomToLevel()
{
    zoomLevel = MAX_LEVEL - log2(zoom);
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
    QGraphicsPixmapItem *mark = new QGraphicsPixmapItem;
    mark->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    if(type == MARK_REPO)
    {
        mark->setPixmap(QPixmap(":/images/repo_marker_48.png"));
        mark->setOffset(p.x()-ICON_SIZE/2+1, p.y()-ICON_SIZE);
        qDebug() << mapFromScene(p);
        markers.push_front(mark);
    }
    else if(type ==MARK_POINT)
    {
        mark->setPixmap(QPixmap(":/images/map_marker_48.png"));
        mark->setOffset(p.x()-ICON_SIZE/2+1, p.y()-ICON_SIZE);
        qDebug() << mapFromScene(p);
        markers.push_back(mark);
    }
    scene->addItem(mark);
    scene->update();
}

void MapView::removeMarker(int index)
{
    scene->removeItem(markers.at(index));
    scene->update();
    QGraphicsPixmapItem *p = markers.takeAt(index);
    delete p;
}

void MapView::clearMarker()
{
    for(auto n : markers)
    {
        scene->removeItem(markers[0]);
        QGraphicsPixmapItem *p = markers.takeFirst();
        delete p;
    }
    scene->update();
}


/*******************************************
 **                             路径相关                             **
 ******************************************/

void MapView::drawPath(DeliveryPath *path)
{
    Polyline *line = new Polyline;
    for(auto p : path->m_path)
        line->addPoint(vertexs[p.index].x(), vertexs[p.index].y());
    line->setBounds();
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(5);
    pen.setCosmetic(true);
    line->setPen(pen);

    scene->addItem(line);
    paths.append(line);

    QGraphicsPixmapItem *car = new QGraphicsPixmapItem;
    car->setZValue(1);
    car->setPixmap(QPixmap(":/images/truck_icon_48.png"));
    car->setOffset(vertexs[0].x()-ICON_SIZE/2, vertexs[0].y()-ICON_SIZE/2);
    cars.push_back(car);
    scene->addItem(car);
    scene->update();
}

void MapView::removePath(int index)
{
    scene->removeItem(paths.at(index));
    Polyline *l = paths.takeAt(index);
    delete l;

    scene->removeItem(cars.at(index));
    QGraphicsPixmapItem *p = cars.takeAt(index);
    delete p;
}

void MapView::updatePath(int index, DeliveryPath *path)
{
    if(path->m_pos > 1)
    {
        Polyline *line = new Polyline;
        line->addPoint(vertexs[path->m_path[path->m_pos-2].index].x(),vertexs[path->m_path[path->m_pos-2].index].y());
        line->addPoint(vertexs[path->m_path[path->m_pos-1].index].x(),vertexs[path->m_path[path->m_pos-1].index].y());
        line->setBounds();
        QPen pen;
        pen.setColor(Qt::blue);
        pen.setWidth(5);
        pen.setCosmetic(true);
        line->setPen(pen);

        scene->addItem(line);
        cars[index]->setOffset(vertexs[path->m_path[path->m_pos-1].index].x()-ICON_SIZE/2,
                 vertexs[path->m_path[path->m_pos-1].index].y()-ICON_SIZE/2);
        scene->update();
    }
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
    QAction *action4 = menu.addAction("计算路径");

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
    QStringList nameList;

    for(auto n : selectedPoints)
    {
        //获得途径点名称
        nameList << n->getName();

        double min = 1000000000.0;
        long index = -1;
        for(auto iter = vertexs.begin(); iter != vertexs.end(); ++iter)
        {
            double len = getLen(*iter, n->points[0]);
            if(len < min)
            {
                min = len;
                index = (iter-vertexs.begin());
            }
        }
        points.push_back(index);
    }
    graph->BestPath(points, path);
//    double a;
//    graph->GetDist(points[0], points[1], path, a);

    //释放selectedPoints中所有元素
    selectedPoints.clear();
    clearMarker();
    repoSelected  = false;
    setSelectMode(false);

    //发送信号
    emit transferData(points, path, nameList);

}


/**********************************
 **                  其他私有槽函数                **
 **********************************/
void MapView::zoomOut()
{
    if(zoomLevel > MIN_LEVEL) // 缩小视图
    {
        --zoomLevel;
        levelToZoom(zoomLevel);
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
        levelToZoom(zoomLevel);
    else
        levelToZoom(level);
    layerVisible(zoom);
}

void MapView::zoomIn()
{
    if(zoomLevel < MAX_LEVEL)
    {
        ++zoomLevel;
        levelToZoom(zoomLevel);
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
