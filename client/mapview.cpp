#include "../network/commands.h"
#include "../network/packet.h"
#include "mapview.h"
#include "pixmapitem.h"
#include <string>
#include <cstdio>
#include <QFont>
#include <QDir>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QBoxLayout>
#include <QWheelEvent>
#include <QMessageBox>
#include <QTransform>
#include <QDebug>

#define MAX(a, b) ((a)>(b)?(a):(b))

MapView::MapView(QWidget *parent, ClientSocket *cli)
    : QGraphicsView(parent)
    , client(cli)
    , preLoaded(false)
{
    scene = new QGraphicsScene(this);
    zoomInBtn = new QPushButton(this);
    zoomOutBtn = new QPushButton(this);

    QFont font;
//    font.setBold(true);
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

//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setHorizontalScrollBarPolicy(Qt::S crollBarAlwaysOff);
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
    int cornerX = bound.x() - paddingH;
    int cornerY = bound.y() - paddingV;
    int step = bound.height() / pow(2, zoomLevel-MIN_LEVEL);

    int right = mapToScene(sizeHint().width(), sizeHint().height()).x();
    int bottom = mapToScene(sizeHint().width(), sizeHint().height()).y();

//    qDebug() << (int)mapToScene(0, 0).y() << cornerY << bottom << step;
//    qDebug() << (int)mapToScene(0, 0).x() << cornerX << right << step;

    QString s("tmp");
    makeDir(s);
    //*********增加判断切片是否已显示
    for(int top = MAX(mapToScene(0, 0).y(), cornerY); top <= bottom; top += step - (top - cornerY) % step)
    {
        int row = (top - cornerY) / step;
        for(int left = MAX(mapToScene(0, 0).x(), cornerX); left <= right; left += step - (left - cornerX) % step)
        {
            int col = (left - cornerX) / step;
            QString fileName = QString("tmp/%1_%2_%3.png").arg(zoomLevel).arg(row).arg(col);
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
            pix->setOffset(left, top);
            scene->addItem(pix);
//            pix->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
//            pix->setTransformationMode(Qt::SmoothTransformation);
//            qDebug() << pix->boundingRect();
        }
    }
    //************增加缓冲保护（缩放动画显示，再加载图片）

    return 0;
}

int MapView::getSymbols()
{

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
    qDebug() << center;
    qDebug() << mapToScene(viewport()->rect().center());
    if(center == mapToScene(viewport()->rect().center()))
        return;
    qDebug() <<"view changed";
    getTiles();
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
    }
}
