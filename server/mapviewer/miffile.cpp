#include "miffile.h"
#include <QDebug>
#include <QTextCodec>

// 指定字符集
QTextCodec *codec = QTextCodec::codecForName("GBK");

MifFile::MifFile()
    : srcFile(NULL)
{
}

int MifFile::openFile(const char *fileName)
{
    // 打开地图文件
     if ((srcFile = IMapInfoFile::SmartOpen(fileName)) == NULL)
    {
         qDebug() << "cannot open file";
        return -1;
    }
    return 0;
}

int MifFile::closeFile()
{
    if(srcFile->Close() != 0)
        return -1;
    delete srcFile;
    return 0;
}

// 获取图形信息
int MifFile::getLayerDefn(MapLayer *layer)
{
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
                shape = new Region;
            else if(featureClass == TABFCPolyline)
                shape = new Polyline;
            else if(featureClass == TABFCPoint)
                shape = new Point;
            else
                return -1;

            shape->setAutoLabel(layer->autoLabel);
            getStyle(layer, shape, featureClass);
//            getStyle(feature, shape, featureClass);
            // get coordinates of each vertexs of the feature
            getCoord(feature, shape, featureClass);
            // set boudingrect for every feature
            shape->setBounds();
            getField(feature, shape);
            layer->addItem(shape, featureClass);
        }
        else
        {
            qDebug() << "cannot read feature";
            return -1;
        }
    }
    return 0;
}

void MifFile::getStyle(MapLayer *layer, MapShape *shape, TABFeatureClass featureClass)
{
    switch(featureClass)
    {
    case TABFCPoint:
        break;
    case TABFCRegion:
        static_cast<Region*>(shape)->setPen(layer->m_pen);
        static_cast<Region*>(shape)->setBrush(layer->m_brush);
        break;
    case TABFCPolyline:
        static_cast<Polyline*>(shape)->setPen(layer->m_pen);
        break;
    }
}

void MifFile::getStyle(TABFeature *feature, MapShape *shape, TABFeatureClass featureClass)
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
        static_cast<Region*>(shape)->setPen(pen);

        QBrush brush(Qt::white);
        TABRegion *region = (TABRegion *)feature;
        brush.setColor(region->GetBrushFGColor());
        static_cast<Region*>(shape)->setBrush(brush);
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
        static_cast<Polyline*>(shape)->setPen(pen);
        break;
    }
    }
}

void MifFile::getCoord(TABFeature *feature, MapShape *shape, TABFeatureClass featureClass)
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
                coordTransform(x, y);
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
                coordTransform(x, y);
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
        coordTransform(x, y);
        shape->addPoint(x, y);
        break;
    }
    default: break;
    }
}

void MifFile::getField(TABFeature *feature, MapShape *shape)
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
