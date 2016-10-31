#include "gstfile.h"
#include "maplayer.h"
#include <QPen>
#include <QBrush>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDebug>

QFile gstFile;
QTextStream in;
qint64 pre_pos = -1;

GstFile::GstFile()
{
}

int GstFile::openFile(const char *filePath)
{
    gstFile.setFileName(filePath);
    if(!gstFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "cannot open gst file";
        return -1;
    }
    in.setDevice(&gstFile);
    return 0;
}

int GstFile::closeFile()
{
    gstFile.close();
    return 0;
}

void GstFile::getGeoDefn(QPointF &center, double &zoomLevel)
{
    in.reset();

    QString line, value;
    QStringList ss;
    while(in.readLineInto(&line) && line != "\"\\TABLE\" = \"\"")
    {
        ss = line.split('"', QString::SkipEmptyParts);
        if(ss.at(0) == "\\GEOSET\\CENTER")
        {
            value = ss.at(2);
            ss = value.split(',');
            double x = ss.at(0).toDouble();
            double y = ss.at(1).toDouble();
            coordTransform(x, y);
            center.setX(x);
            center.setY(y);
        }
        else if(ss.at(0) == "\\GEOSET\\ZOOMLEVEL")
        {
            value = ss.at(2);
            zoomLevel = value.toDouble();
        }
    }// while
}

int GstFile::getTableDefn(QList<MapLayer *> &layers)
{
//    resetReading();

    QString line;
    QString key, value;
    QStringList ss;
    int currTableId = 0;
    while((currTableId = getNextTableId(currTableId)) != -1)
    {
        MapLayer *layer = new MapLayer;
        layer->index = currTableId-1;
        QPen pen;
        QBrush brush(Qt::white);
        while(in.readLineInto(&line) &&
              (line != QString("\"\\TABLE\\%1\" = \"\"").arg(currTableId+1) &&
              line != QString("\"\\TABLE\\%1\" = \"\"").arg(currTableId+2)))
        {
            pre_pos = in.pos();
            ss = line.split('"', QString::SkipEmptyParts);
            key = ss.at(0);
            if(key.contains("DESCRIPTION"))
            {
                layer->name = ss.at(2);
            }
            else if(key.contains("AUTOLABEL"))
            {
                layer->autoLabel = (ss.at(2) == "TRUE");
            }
            else if(key.contains("DISPLAY\\BRUSH\\Forecolor"))
            {
                value = ss.at(2);
                brush.setColor(value.toInt());
            }
            else if(key.contains("DISPLAY\\LINEPEN\\LineWidth"))
            {
                value = ss.at(2);
                pen.setWidthF(value.toDouble()/5);
            }
            else if(key.contains("DISPLAY\\LINEPEN\\Color"))
            {
                value = ss.at(2);
                pen.setColor(value.toInt());
            }
            else if(key.contains("ZOOM\\MIN"))
            {
                value = ss.at(2);
                layer->minZoom = value.toDouble();
            }
            else if(key.contains("ZOOM\\MAX"))
            {
                value = ss.at(2);
                layer->maxZoom = value.toDouble();
            }
        }
        layer->setPen(pen);
        layer->setBrush(brush);
        in.seek(pre_pos);
        //TODO*/
        layers.append(layer);
    }
}

void GstFile::resetReading()
{
    in.seek(0);
    QString line;
    while(in.readLineInto(&line) && line != "\"\\TABLE\" = \"\"")
        continue;
}

int GstFile::getNextTableId(int id)
{
    QString line;
    QString title = QString("\"\\TABLE\\%1\" = \"\"").arg(id+1);
    QString title2 = QString("\"\\TABLE\\%1\" = \"\"").arg(id+2);
    while(in.readLineInto(&line))
    {
        if(line == title)
            return id+1;
        else if(line == title2)
            return id+2;
    }
    return -1;
}
