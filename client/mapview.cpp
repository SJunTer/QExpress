#include "../network/commands.h"
#include "../network/packet.h"
#include "mapview.h"
#include "pixmapitem.h"
#include "tileloader.h"
#include <string>
#include <cstdio>
#include <QFont>
#include <QDir>
#include <QMutex>
#include <QMutexLocker>
#include <QPixmap>
#include <QThread>
#include <QWheelEvent>
#include <QMessageBox>
#include <QDebug>


MapView::MapView(QWidget *parent, ClientSocket *cli) :
    QGraphicsView(parent),
    client(cli),
    tileLoading(false),
    threadWaiting(false),
    preLoaded(false)
{
    scene = new QGraphicsScene(this);

    for(int i = MIN_LEVEL; i <= MAX_LEVEL; ++i)
    {
        QGraphicsItemGroup *group = new QGraphicsItemGroup;
        layers.push_back(group);
        scene->addItem(group);
    }

//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::ScrollHandDrag); //手型拖动

    // preload geographical informatioin
    if(getPreGeoInfo() != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("无法初始化地图信息！"), QMessageBox::Ok);
    }
    preLoaded = true;
    scene->setSceneRect(bound);
    setScene(scene);
    centerOn(center);

    QString s("tmp");
    makeDir(s);

    getTiles();

}

MapView::~MapView()
{
    for(auto n : layers)
    {
        QGraphicsItemGroup *g = layers.takeFirst();
        delete g;
    }
}



/*********************************
 *                     Private Functions                     *
 * ********************************/
int MapView::getPreGeoInfo()
{
    std::string data;
    commandType cmd;

    data ="0";
    if(sendPacket(client->sock(), cmd_preLoad, data) != 0)
        return SEND_ERROR;
    if(recvPacket(client->sock(), &cmd, data) != 0)
        return RECV_ERROR;
    zoomLevel = fromByteString<int>(data);
    double x, y, width, height, centerX, centerY;
    x = fromByteString<double>(data);
    y = fromByteString<double>(data);
    width = fromByteString<double>(data);
    height = fromByteString<double>(data);
    bound = QRectF(x, y, width, height);
    centerX = fromByteString<double>(data);
    centerY = fromByteString<double>(data);
    center = QPointF(centerX, centerY);
    qDebug() << center;
    qDebug() << mapToScene(rect().center());
    return 0;
}

//创建文件夹
void MapView::makeDir(QString &folderName)
{
    QDir dir(folderName);
    if(!dir.exists())
    {
        dir.mkdir(dir.absolutePath());
    }
}



// 加载地图
int MapView::getTiles()
{
    if(!preLoaded)
        return -1;

    // 切片线程
    thread = new QThread;
    QPoint corner = this->viewport()->rect().bottomRight();
    int viewRight = mapToScene(corner).x();
    int viewBottom = mapToScene(corner).y();
    int viewLeft = mapToScene(0, 0).x();
    int viewTop = mapToScene(0, 0).y();
    QRect viewRect(viewLeft, viewTop, viewRight-viewLeft, viewBottom-viewTop);
    TileLoader *tileLoader = new TileLoader(client->sock(), zoomLevel, bound, viewRect, tiles);
    tileLoader->moveToThread(thread);
    connect(thread, SIGNAL(started()), tileLoader, SLOT(doWork()));
    connect(tileLoader, SIGNAL(taskFinished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), tileLoader, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    connect(tileLoader, SIGNAL(taskFinished()), this, SLOT(setLoadFlag()));
    connect(tileLoader, SIGNAL(drawPixmap(QString,int,int,int)), this, SLOT(drawPixmap(QString,int,int,int)));
    connect(tileLoader, SIGNAL(drawSymbol(QString,int,int,int)), this, SLOT(drawSymbol(QString,int,int,int)));
    connect(this, SIGNAL(stopLoading()), tileLoader, SLOT(stop()), Qt::DirectConnection);

    if(tileLoading)
    {
//        qDebug() << "set thread waiting";
        threadWaiting = true;
        emit stopLoading();
    }
    else
    {
        threadWaiting = false;
        tileLoading = true;
        thread->start();
    }

    //************增加缓冲保护（缩放动画显示，再加载图片）

    return 0;
}


/*********************************
 *                       Event Overload                      *
 * ********************************/
void MapView::wheelEvent(QWheelEvent *event)
{
    center = mapToScene((event->pos()+rect().center())/2);
    // zoomLevel: 8~19
    if(event->delta() < 0)  //如果鼠标滚轮远离使用者，则delta()返回正值
        zoomOut();
    else
        zoomIn();
}



void MapView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    QGraphicsView::mouseReleaseEvent(event);
    center = mapToScene(viewport()->rect().center());
    centerOn(center);
    getTiles();
    setLayerVisible();
    scene->update();
}

/*
void MapView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    getTiles();
    setLayerVisible();
}*/

/*
void MapView::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
//    qDebug() << event->buttons() & Qt::

    center = mapToScene(viewport()->rect().center());
    centerOn(center);
    QGraphicsView::mouseMoveEvent(event);
    getTiles();
    setLayerVisible();
}*/



/*********************************
 *                       Private Slots                          *
 * ********************************/
void MapView::setLoadFlag()
{
    tileLoading = false;
//    qDebug() << "set load flag";
//    qDebug() << "thread waiting: " << threadWaiting;
    if(threadWaiting)
    {
        threadWaiting = false;
        tileLoading = true;
        thread->start();
    }
}

/*********************************
 *                         Public Slots                          *
 * ********************************/

// 放大视图
void MapView::zoomIn()
{
    if(zoomLevel < MAX_LEVEL)
    {
        ++zoomLevel;
        scale(2,2);
        centerOn(center);
        getTiles();
        setLayerVisible();
    }
}

// 缩小视图
void MapView::zoomOut()
{
    if(zoomLevel > MIN_LEVEL)
    {
        --zoomLevel;
        scale(0.5,0.5);
        centerOn(center);
        getTiles();
        setLayerVisible();
    }
}

void MapView::setLayerVisible()
{
    for(int i = MIN_LEVEL; i <= MAX_LEVEL; ++i)
    {
        if(i == zoomLevel)
            layers[i-MIN_LEVEL]->setVisible(true);
        else
            layers[i-MIN_LEVEL]->setVisible(false);
    }
}


void MapView::drawPixmap(QString fileName, int lv, int x, int y)
{
//    qDebug() << fileName << lv << x << y;
    MapTileItem *pix = new MapTileItem;
    QPixmap p(fileName);
    pix->setPixmap(p);
    pix->setOffset(x, y);
//    qDebug() << x << y;
    tiles.push_back(fileName);
    layers[lv-MIN_LEVEL]->addToGroup(pix);
    scene->update();
}

void MapView::drawSymbol(QString symbolName, int lv, int x, int y)
{
//    qDebug() << symbolName << lv << x << y;
    SymbolItem *symbol = new SymbolItem;
    QPixmap p(";/images/symbol_24.png");
    symbol->setPixmap(p);
    symbol->setCoord(x, y);
    symbol->setTitile(symbolName);
    symbol->setZValue(2);
    layers[lv-MIN_LEVEL]->addToGroup(symbol);
    scene->update();
}
