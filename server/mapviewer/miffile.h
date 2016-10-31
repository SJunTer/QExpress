#ifndef MIFFILE_H
#define MIFFILE_H

#include "mitab.h"
#include "mapshape.h"
#include "maplayer.h"

class MifFile
{
public:
    MifFile();
    int openFile(const char *fileName);
    int closeFile();
    int getLayerDefn(MapLayer *layer);

private:
    void getStyle(MapLayer *layer, MapShape *shape, TABFeatureClass featureClass);
    void getStyle(TABFeature *feature, MapShape *shape, TABFeatureClass featureClass);
    void getCoord(TABFeature *feature, MapShape *shape, TABFeatureClass featureClass);
    void getField(TABFeature *feature, MapShape *shape);

    IMapInfoFile *srcFile;

};

#endif // MIFFILE_H
