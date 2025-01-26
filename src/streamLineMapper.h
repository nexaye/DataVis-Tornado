//
// Created by Paul on 10.07.2022.
//

#ifndef DV_STREAMLINEMAPPER_H
#define DV_STREAMLINEMAPPER_H


#include <QVector3D>
#include <QVector>
#include "Util.h"
#include "flowdatasource.h"

class streamLineMapper {
public:
    explicit streamLineMapper(FlowDataSource *source);
    QVector<QVector3D> computeStreamLines(QVector<QVector3D> seedPoints, int method, float stepSize);
    void setDataSource(FlowDataSource *source);
private:
    streamLineMapper();
    QVector3D EulerIntegration(QVector3D position, float stepSize);
    QVector3D KuttaSecondOrderIntegration(QVector3D position, float stepSize);
    QVector3D KuttaFourthOrderIntegration(QVector3D position, float stepSize);
    QVector<PointWind> findCell(QVector3D position);
    QVector3D interpolateWindVector(QVector3D position);
    PointWind singleInterpolation(PointWind p1, PointWind p2, float point, int component);
    FlowDataSource *source;
};

#endif //DV_STREAMLINEMAPPER_H
