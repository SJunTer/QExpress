#include "tileloader.h"
#include "../network/commands.h"
#include "../network/packet.h"
#include <QPixmap>
#include <QFile>
#include <QDebug>

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))

#define MIN_LEVEL 8
#define MAX_LEVEL 18

TileLoader::TileLoader(int sock, int lv, QRectF sr, QRectF vr, QVector<QString> &t)
    : sockfd(sock)
    , zoomLevel(lv)
    , sceneRect(sr)
    , viewRect(vr)
    , tiles(t)
    , stopped(false)
{
    qRegisterMetaType<QString>("QString");
}

//执行切片任务
void TileLoader::doWork()
{
//    qDebug() << "start work";

    int paddingH = (256-(int)(sceneRect.width())%256)/2;
    int paddingV = (256-(int)(sceneRect.height())%256)/2;
    int sceneLeft = sceneRect.x() - paddingH;
    int sceneTop = sceneRect.y() - paddingV;
    int sceneRight = sceneRect.x() + sceneRect.width();
    int sceneBottom = sceneRect.y() +sceneRect.height();
    int step = (sceneRect.height()+2*paddingV) / pow(2, zoomLevel-MIN_LEVEL);

    int viewRight = viewRect.right();
    int viewBottom = viewRect.bottom();
    int viewLeft = viewRect.left();
    int viewTop = viewRect.top();

//    qDebug() << sceneRect.width() << sceneRect.height();
//    qDebug() << sceneLeft << sceneRight << sceneTop << sceneBottom;
//    qDebug() << viewLeft << viewRight << viewTop << viewBottom;
//    qDebug() << mapFromScene(viewLeft, viewTop) << mapFromScene(viewRight, viewBottom);

    viewTop = MAX(viewTop, sceneTop);
    viewLeft = MAX(viewLeft, sceneLeft);
    if(zoomLevel == 9)
        int n = 0;
    stopped = false;

    std::string data;
    commandType cmd;
    for(int y = viewTop - (viewTop-sceneTop)%step; !stopped && y < MIN(viewBottom, sceneBottom); y += step)
    {
        int row = (y-sceneTop) / step;
        for(int x = viewLeft - (viewLeft-sceneLeft)%step; !stopped && x < MIN(viewRight, sceneRight); x += step)
        {
            int col = (x-sceneLeft) / step;
            QString fileName = QString("tmp/%1_%2_%3.png").arg(zoomLevel).arg(row).arg(col);

            //if(emit tileLoaded(fileName))
            if(tiles.contains(fileName))    // 如果切片已加载
                continue;

            qDebug() << fileName;

            ///////////////加载地图标记
            data.clear();
            data += toByteString(zoomLevel) + toByteString(x) + toByteString(y) + toByteString(step);
            if(sendPacket(sockfd, cmd_getSymbol, data) != 0)
            {
                qDebug() << "network error";
                emit taskFinished();
                return;
            }
            if(recvPacket(sockfd, &cmd, data) != 0)
            {
                qDebug() << "network error";
                emit taskFinished();
                return;
            }
            int cnt = fromByteString<int>(data);
            QString s;
            for(int i = 0; i < cnt; ++i)
            {
                int len = fromByteString<int>(data);
                s = QString::fromStdString(fromByteString(data, len));
                double s_x = fromByteString<double>(data);
                double s_y = fromByteString<double>(data);
                emit drawSymbol(s, zoomLevel, s_x, s_y);
            }

            /////////////加载地图瓦片
            data.clear();
            data += toByteString(zoomLevel) + toByteString(row) + toByteString(col);
            if(sendPacket(sockfd, cmd_getTile, data) != 0)
            {
                qDebug() << "network error";
                emit taskFinished();
                return;
            }
            if(recvPacket(sockfd, &cmd, data) != 0)
            {
                qDebug() << "network error";
                emit taskFinished();
                return;
            }
            int len = fromByteString<int>(data);
//            qDebug() << len;
            if(len == 0)    // 没有图片则跳过
                continue;
            FILE *fp;
            if((fp = fopen(fileName.toStdString().c_str(), "wb")) == NULL)
                continue;
            for(int i = 0; i < len; ++i)
                fwrite((char*)&data[i], sizeof(char), 1, fp);
            fclose(fp);
            emit drawPixmap(fileName, zoomLevel, x, y);

        }//for x,y
    }


    emit taskFinished();
}
