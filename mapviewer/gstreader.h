#ifndef GSTREADER_H
#define GSTREADER_H

#include <QList>

class MapLayer;
class QPointF;

class GstFile
{
public:
    GstFile();

    int openFile(const char* filePath);
    int closeFile();
    int getTableDefn(QList<MapLayer *> &layers);
    void getGeoDefn(QPointF &center, double &zoomLevel);

private:
    void resetReading();
    int getNextTableId(int id);

};

#endif // GSTREADER_H
