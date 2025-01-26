//
// Created by Paul on 06.06.2022.
//

#ifndef DV_HORIZONTALSLICETOCONTOURLINEMAPPER_H
#define DV_HORIZONTALSLICETOCONTOURLINEMAPPER_H


#include <QVector3D>
#include <QVector>
#include "flowdatasource.h"
#include <cstdint>
#include "bitset"
#include "Util.h"

class HorizontalSliceToContourLineMapper {
public:
    explicit HorizontalSliceToContourLineMapper(FlowDataSource *source);
    QVector<QVector3D> mapSliceToContourLineSegments(int slice, int component, float isovalue);
    void setDataSource(FlowDataSource *source);
private:
    HorizontalSliceToContourLineMapper();
    float interpolate(Point p1, Point p2, float isovalue, bool useY);
    float interpolateX(Point p1, Point p2, float point, bool useY);
    float interpolateMidPoint(Point points[]);
    FlowDataSource *source;


};


#endif //DV_HORIZONTALSLICETOCONTOURLINEMAPPER_H
