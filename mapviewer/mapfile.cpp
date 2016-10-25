#include "mapfile.h"
#include <QDebug>
#include <QTextCodec>

// 指定字符集
QTextCodec *codec = QTextCodec::codecForName("GBK");

MapFile::MapFile()
{
}

int MapFile::openFile(const char *fileName)
{
    // 打开地图文件
     if ((srcFile = IMapInfoFile::SmartOpen(fileName)) == NULL)
    {
         qDebug() << "cannot open file";
        return -1;
    }
    return 0;
}

int MapFile::closeFile()
{
    srcFile->Close();
    delete srcFile;
}

// 获取图形信息
int MapFile::getLayerInfo(MapLayer *layer)
{
    layer->getFeatureCount(srcFile->GetFeatureCount(TRUE));

    srcFile->ResetReading();
    MapShape *shape;
    TABFeature *feature;

    int featureId = -1;
    while ((featureId = srcFile->GetNextFeatureId(featureId)) != -1)
    {
        feature = srcFile->GetFeatureRef(featureId);
        if (feature)
        {
            TABFeatureClass featureClass = feature->GetFeatureClass();
            if (featureClass == TABFCRegion)
            {
                shape = new Region;
            }
            else if(featureClass == TABFCPolyline)
            {
                shape = new Polyline;
            }
            else if(featureClass == TABFCPoint)
            {
                shape = new Point;
            }
            else
            {
                shape = new Region;
            }

            shape->setIndex(featureId);
            // get coordinates of each vertexs of the feature
            getStyle(feature, shape, featureClass);

            getCoord(feature, shape, layer, featureClass);
            // set boudingrect for every feature
            shape->setBounds();
            getField(feature, shape);
            layer->addShape(shape, featureClass);

        }
        else
        {
            qDebug() << "cannot read feature";
            return -1;
        }
    }
    return 0;
}

void MapFile::getStyle(TABFeature *feature, MapShape *shape, TABFeatureClass featureClass)
{
    switch(featureClass)
    {
    case TABFCPoint:
        break;

    case TABFCRegion:
    {
        QPen pen;
        ITABFeaturePen *p = (TABRegion *)feature;
        pen.setColor(p->GetPenColor());
        pen.setWidth(p->GetPenWidthMIF());
        if(p->GetPenPattern() == 2)
            pen.setStyle(Qt::SolidLine);
        else
            pen.setStyle(Qt::DashLine);
        shape->setPen(pen);

        QBrush brush(Qt::white);
        TABRegion *region = (TABRegion *)feature;
        brush.setColor(region->GetBrushFGColor());
        //b->GetBrushBGColor();
       // b->GetBrushPattern();
        shape->setBrush(brush);
        break;
    }
    case TABFCPolyline:
    {
        QPen pen;
        ITABFeaturePen *p = (TABPolyline *)feature;
        pen.setColor(p->GetPenColor());
        pen.setWidth(p->GetPenWidthMIF());
        if(p->GetPenPattern() == 2)
            pen.setStyle(Qt::SolidLine);
        else
            pen.setStyle(Qt::DashLine);
        shape->setPen(pen);
        break;
    }
    }
}

void MapFile::getCoord(TABFeature *feature, MapShape *shape, MapLayer *layer, TABFeatureClass featureClass)
{
    switch(featureClass)
    {
    case TABFCRegion:
    {
        TABRegion *region = (TABRegion *)feature;
        for (int ringId = 0; ringId < region->GetNumRings(); ++ringId)
        {
            OGRLinearRing *ring = region->GetRingRef(ringId);
            for (int pointId = 0; pointId < ring->getNumPoints(); ++pointId)
            {
                double x = ring->getX(pointId);
                double y = ring->getY(pointId);
                layer->coordTransform(x, y);
                shape->addPoint(x, y);
            }
        }
        break;
    }
    case TABFCPolyline:
    {
        TABPolyline *polyline = (TABPolyline *)feature;
        for (int partId = 0; partId < polyline->GetNumParts(); ++partId)
        {
            OGRLineString *line = polyline->GetPartRef(partId);
            for (int pointId = 0; pointId < line->getNumPoints(); ++pointId)
            {
                double x = line->getX(pointId);
                double y = line->getY(pointId);
                layer->coordTransform(x, y);
                shape->addPoint(x, y);
            }
        }
        break;
    }
    case TABFCPoint:
    {
        OGRGeometry *geom = feature->GetGeometryRef();
        OGRPoint *point = (OGRPoint *)geom;
        double x = point->getX();
        double y = point->getY();
        layer->coordTransform(x, y);
        shape->addPoint(x, y);
        break;
    }
    }
}

void MapFile::getField(TABFeature *feature, MapShape *shape)
{
    // 获取图层信息
    QString fieldName, fieldType, fieldContent;
    OGRFeatureDefn *featureDefn = srcFile->GetLayerDefn();

    for (int fieldId = 0; fieldId < featureDefn->GetFieldCount(); ++fieldId)
    {
        OGRFieldDefn *fieldDefn = featureDefn->GetFieldDefn(fieldId);

        fieldName = fieldDefn->GetNameRef();
        fieldType = fieldDefn->GetFieldTypeName(fieldDefn->GetType());
        fieldContent = codec->toUnicode(feature->GetFieldAsString(fieldId));
        shape->addField(fieldName, fieldType, fieldContent);
    }
}
