#ifndef MAPFILE_H
#define MAPFILE_H

#include "mitab.h"
#include "mapshape.h"
#include "maplayer.h"

class MapFile
{
public:
    MapFile();
    int openFile(const char *fileName);
    int closeFile();
    int getLayerInfo(MapLayer *layer);

private:
    void getStyle(TABFeature *feature, MapShape *shape, TABFeatureClass featureClass);
    void getCoord(TABFeature *feature, MapShape *shape, MapLayer *layer, TABFeatureClass featureClass);
    void getField(TABFeature *feature, MapShape *shape);

    IMapInfoFile *srcFile = NULL;

};

#endif // MAPFILE_H
