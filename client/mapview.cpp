#include "commands.h"
#include "packet.h"
#include "mapview.h"
#include "pixmapitem.h"
#include <string>
#include <cstdio>
#include <QFont>
#include <QDir>
#include <QPixmap>
#include <QWheelEvent>
#include <QMessageBox>
#include <QDebug>

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))

MapView::MapView(QWidget *parent, ClientSocket *cli)
    : QGraphicsView(parent)
    , client(cli)
    , preLoaded(false)
{
    scene = new QGraphicsScene(this);

    for(int i = MIN_LEVEL; i <= MAX_LEVEL; ++i)
    {
        QGraphicsItemGroup *group = new QGraphicsItemGroup;
        basemap.push_back(group);
        scene->addItem(group);
    }

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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

    getTiles();
}

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

void MapView::makeDir(QString &folderName)
{
    QDir dir(folderName);
    if(!dir.exists())
    {
        dir.mkdir(dir.absolutePath());
    }
}

bool MapView::isExisted(QString &fileName)
{
    QFile file(fileName);
    return file.exists();
}

int MapView::getTiles()
{
    if(!preLoaded)
        return -1;

    std::string data;
    commandType cmd;

    int paddingH = (256-(int)(bound.width())%256)/2;
    int paddingV = (256-(int)(bound.height())%256)/2;
    int sceneLeft = bound.x() - paddingH;
    int sceneTop = bound.y() - paddingV;
    int sceneRight = bound.x() + bound.width() + paddingH;
    int sceneBottom = bound.y() +bound.height() + paddingV;
    int step = (bound.height()+2*paddingV) / pow(2, zoomLevel-MIN_LEVEL);

    QPoint corner = this->viewport()->rect().bottomRight();
    int viewRight = mapToScene(corner).x();
    int viewBottom = mapToScene(corner).y();
    int viewLeft = mapToScene(0, 0).x();
    int viewTop = mapToScene(0, 0).y();

    qDebug() << step;
    qDebug() << bound.width() << bound.height();
    qDebug() << sceneLeft << sceneRight << sceneTop << sceneBottom;
    qDebug() << viewLeft << viewRight << viewTop << viewBottom;

    QString s("tmp");
    makeDir(s);
    //*********增加判断切片是否已显示
    viewTop = MAX(viewTop, sceneTop);
    viewLeft = MAX(viewLeft, sceneLeft);
    for(int y = viewTop - (viewTop-sceneTop)%256; y < MIN(viewBottom, sceneBottom); y += step)
    {
        int row = (y-sceneTop) / step;
        for(int x = viewLeft - (viewLeft-sceneLeft)%256; x < MIN(viewRight, sceneRight); x += step)
        {
            int col = (x-sceneLeft) / step;
            QString fileName = QString("tmp/%1_%2_%3.png").arg(zoomLevel).arg(row).arg(col);
            qDebug() << fileName;
            PixmapItem *pix = new PixmapItem;

            if(!isExisted(fileName))
            {
//                qDebug() << fileName<<"not existed";
                data.clear();
                data += toByteString(zoomLevel) + toByteString(row) + toByteString(col);
                if(sendPacket(client->sock(), cmd_getTile, data) != 0)
                    return SEND_ERROR;
                if(recvPacket(client->sock(), &cmd, data) != 0)
                    return RECV_ERROR;
                int len = fromByteString<int>(data);
                if(len == 0)
                    continue;
                FILE *fp;
                if((fp = fopen(fileName.toStdString().c_str(), "wb")) == NULL)
                {
                    qDebug() << "cannot create file";
                    continue;   //无法创建文件
                }
                for(int i = 0; i < len; ++i)
                {
                    fwrite((char*)&data[i], sizeof(char), 1, fp);
                }
                fclose(fp);
            }
            QPixmap p(fileName);
            pix->setPixmap(p);
            pix->setOffset(x, y);
            basemap[zoomLevel-MIN_LEVEL]->addToGroup(pix);
        }
    }
    //************增加缓冲保护（缩放动画显示，再加载图片）

    return 0;
}

int MapView::getSymbols()
{

}

void MapView::setLayerVisible()
{
    for(int i = MIN_LEVEL; i <= MAX_LEVEL; ++i)
    {
        if(i == zoomLevel)
            basemap[i-MIN_LEVEL]->setVisible(true);
        else
            basemap[i-MIN_LEVEL]->setVisible(false);
    }
}

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
/*   QGraphicsView::mouseReleaseEvent(event);
    qDebug() << center;
    qDebug() << mapToScene(viewport()->rect().center());
    if(center == mapToScene(viewport()->rect().center()))
        return;
    qDebug() <<"view changed";
    getTiles();*/
}

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
