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
#include <QContextMenuEvent>
#include <algorithm>
//#include <cstdio>

MapView::MapView(QWidget *parent = 0)
    : QGraphicsView(parent)
    , zoomLevel(7)
    , selectMode(false)
    , repoSelected(false)
{
//    resize(1280, 720);
//    setStyleSheet("background: transparent");
/*    QPalette p;
    p.setBrush(backgroundRole(), QColor(255,255,255,210));
    setPalette(p);*/

    zoomInBtn = new QPushButton(this);
    zoomOutBtn = new QPushButton(this);
    zoomInBtn->resize(BTN_SIZE,BTN_SIZE);
    zoomInBtn->setText("+");
    zoomOutBtn->resize(BTN_SIZE,BTN_SIZE);
    zoomOutBtn->setText("-");


    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::ScrollHandDrag);//手型拖动

    graph = new Graph;
    scene = new QGraphicsScene();
    setScene(scene);
    mapfile = new MapFile;
    // get name list of *.mif file
    fileInfo = getFileList();
    for(int i = 0; i < fileInfo->count(); ++i)
    {
        MapLayer layer;
        // convert QString to const char *
        mapfile->openFile(fileInfo->at(i).filePath().toStdString().c_str());
        mapfile->getLayerInfo(&layer);
        layers.append(layer);
        // add graphics items to the scene
        layer.drawLayer(scene);
        mapfile->closeFile();
    }
    centerOn(scene->itemsBoundingRect().center());

     // store as algorithm style
    convertData();

    graph->GenerateGraph(vertexs.size(), distances.size(), distances);

}

QFileInfoList *MapView::getFileList()
{
    QDir *dir = new QDir("./maps/");
    // set suffix filter
    QStringList filter;
    filter << "*.mif";
    dir->setNameFilters(filter);
    QFileInfoList *fileInfo = new QFileInfoList(dir->entryInfoList(filter));
    return fileInfo;
}

void MapView::convertData()
{
    for(auto &layer : layers)// 遍历每个图层
    {
        for(auto line : layer.lines)//遍历图层中的每条路线
        {
            for(auto point : line->getPoints())// 遍历每条线上的点
            {
                //生成点集
                vertexs.append(point);
            }
        }
    }
    //重排点集
    generatePoint(vertexs);

    for(auto &layer : layers)// 遍历每个图层
    {
        for(auto line : layer.lines)//遍历图层中的每条路线
        {
            auto points = line->getPoints();
            for(auto iter = points.begin(); iter != points.end()-1; ++iter)// 遍历每条线上的点
            {
                dist d;
                d.vertexA = findPoint(vertexs, *iter, 0, vertexs.size()-1);
                d.vertexB = findPoint(vertexs, *(iter+1), 0, vertexs.size()-1);
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

/*****二分搜索点集返回下表******/
long MapView::findPoint(QVector<QPointF> &points, QPointF point, int xx, int yy)
{
    int m = xx + (yy - xx) / 2;
    if(xx > yy)
    {
        return -1;
    }
    else
    {
        if(points[m].x() == point.x())
        {
            if(points[m].y() == point.y())
            {
                return m;
            }
            else if(points[m].y() > point.y())
            {
                return findPoint(points, point, xx, m - 1);
            }
            else
            {
                return findPoint(points, point, m + 1, yy);
            }
        }
        else if(points[m].x() > point.x())
        {
            return findPoint(points, point, xx, m - 1);
        }
        else
        {
            return findPoint(points, point, m + 1, yy);
        }
    }

}


double MapView::getLen(QPointF &p1, QPointF &p2)
{
    return sqrt(pow((p1.x() - p2.x()), 2) + pow((p1.y() - p2.y()), 2));
}

void MapView::addMarker(QPointF p, int type)
{
    QGraphicsPixmapItem *mark = new QGraphicsPixmapItem;
    if(type == MARK_REPO)
    {
        mark->setPixmap(QPixmap(":/images/gps_map_red_48.png"));
        mark->setOffset(p.x()-ICON_SIZE/2+1, p.y()-ICON_SIZE);
        markers.push_front(mark);
    }
    else if(type ==MARK_POINT)
    {
        mark->setPixmap(QPixmap(":/images/gps_map_blue_48.png"));
        mark->setOffset(p.x()-ICON_SIZE/2+1, p.y()-ICON_SIZE);
        markers.push_back(mark);
    }
    scene->addItem(mark);
}

void MapView::removeMarker(int index)
{
    scene->removeItem(markers.at(index));
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
}

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
    if(path->pos > 1)
    {
        Polyline *line = new Polyline;
        line->addPoint(vertexs[path->m_path[path->pos-2].index].x(),vertexs[path->m_path[path->pos-2].index].y());
        line->addPoint(vertexs[path->m_path[path->pos-1].index].x(),vertexs[path->m_path[path->pos-1].index].y());
        line->setBounds();
        QPen pen;
        pen.setColor(Qt::blue);
        pen.setWidth(5);
        pen.setCosmetic(true);
        line->setPen(pen);

        scene->addItem(line);
        cars[index]->setOffset(vertexs[path->m_path[path->pos-1].index].x()-ICON_SIZE/2,
                 vertexs[path->m_path[path->pos-1].index].y()-ICON_SIZE/2);
        scene->update();
    }
}

void MapView::saveFile(QList<dist> &distances)
{
    FILE *fp;
    fp = fopen("data.dat", "wb");

    for(int i=0;i<distances.size();i++)
    {
        fwrite(&distances[i], sizeof(dist), 1, fp);
    }
    fclose(fp);
}

void MapView::readFile(QList<dist> &distances)
{
    FILE *fp;
    fp = fopen("data.dat", "rb");

    dist d;
    while(fread(&d, sizeof(dist), 1, fp) == 1)
        distances.append(d);
    fclose(fp);
}

/*
void MapView::makeTile(QGraphicsScene *scene)
{

    QPixmap pix(256, 256);
    QPainter painter(&pix);
    int paddingH = (256-(int)(scene->itemsBoundingRect().width())%256)/2;
    int paddingV = (256-(int)(scene->itemsBoundingRect().height())%256)/2;
    qDebug() << scene->itemsBoundingRect();
    qDebug() << paddingH << paddingV;
    for(int level = 0; level < 3; ++level)
    {
        int side = 256/pow(2, level);
        for(int row = 0; row*side < scene->itemsBoundingRect().height()+2*paddingV; ++row)
        {
            for(int col = 0; col*side < scene->itemsBoundingRect().width()+2*paddingH; ++col)
            {
                qDebug() << col*side << scene->itemsBoundingRect().width();
                pix.fill(Qt::white);
                scene->render(&painter, QRectF(),
                              QRect(scene->itemsBoundingRect().x()-paddingH+col*side,
                                    scene->itemsBoundingRect().y()-paddingV+row*side, side, side));
                char filename[100];
                sprintf(filename, "tiles/%d-%d-%d.png", level, row, col);
                qDebug() << filename;
                pix.save(filename);
            }
        }
    }
}
*/

void MapView::wheelEvent(QWheelEvent *event)
{
    QPoint focus;
    focus = (event->pos()+rect().center())/2;
    if(event->delta() < 0)  //如果鼠标滚轮远离使用者，则delta()返回正值
    {
        if(zoomLevel > 1)
        {
            --zoomLevel;
            scale(0.5,0.5);  //视图缩放
            centerOn(mapToScene(focus));
        }
    }
    else
    {
            if(zoomLevel < 7)
            {
                ++zoomLevel;
                scale(2,2);
                centerOn(mapToScene(focus));
            }
     }
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

void MapView::addRepo()
{
    selectedPoints.push_front(tempPoint);
    repoSelected = true;
    addMarker(tempPoint->point, MARK_REPO);
}

void MapView::addPlace()
{
    selectedPoints.push_back(tempPoint);
    addMarker(tempPoint->point, MARK_POINT);
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
            double len = getLen(*iter, n->point);
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

struct cmp
{
    bool operator() (const QPointF a, const QPointF b)
    {
        if(a.x() == b.x())
        {
            return a.y() > b.y();
        }
        else
        {
            return a.x() > b.x();
        }
    }
};

/***将点集排序并消除重复项**/
void MapView::generatePoint(QVector<QPointF> &theVector)
{
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
