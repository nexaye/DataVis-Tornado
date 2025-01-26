//
// Created by Paul on 10.07.2022.
//

#include <cmath>
#include <iostream>
#include "streamLineMapper.h"

streamLineMapper::streamLineMapper(FlowDataSource *source): streamLineMapper() {
    setDataSource(source);
}

streamLineMapper::streamLineMapper() : source(nullptr) {
}

QVector<QVector3D> streamLineMapper::computeStreamLines(QVector<QVector3D> seedPoints, int method, float stepSize) {
    QVector<QVector3D> LineSegments;
    while(!seedPoints.empty())
    {
        QVector3D currentPoint = seedPoints.takeFirst();

        while(!(currentPoint.x() > 31 || currentPoint.x() < 0 || currentPoint.y() > 31 || currentPoint.y() < 0 || currentPoint.z() > 31 || currentPoint.z() < 0))
        {
            LineSegments.append(QVector3D(currentPoint.x()/31,currentPoint.y()/31,currentPoint.z()/31));
            QVector3D temp;
            switch(method)
            {
                case EULER:
                    temp = EulerIntegration(currentPoint, stepSize);
                    currentPoint = QVector3D(temp.x(),temp.y(),temp.z());
                    LineSegments.append(QVector3D(currentPoint.x()/31,currentPoint.y()/31,currentPoint.z()/31));
                    break;
                case KUTTA2:
                    temp = KuttaSecondOrderIntegration(currentPoint, stepSize);
                    currentPoint = QVector3D(temp.x(),temp.y(),temp.z());
                    LineSegments.append(QVector3D(currentPoint.x()/31,currentPoint.y()/31,currentPoint.z()/31));
                    break;
                case KUTTA4:
                    temp = KuttaFourthOrderIntegration(currentPoint, stepSize);
                    currentPoint = QVector3D(temp.x(),temp.y(),temp.z());
                    LineSegments.append(QVector3D(currentPoint.x()/31,currentPoint.y()/31,currentPoint.z()/31));
                    break;
            }
        }
    }
    return LineSegments;
}

void streamLineMapper::setDataSource(FlowDataSource *source) {
    this->source = source;
}

QVector3D streamLineMapper::EulerIntegration(QVector3D position, float stepSize) {
    QVector3D windVector = interpolateWindVector(position);

    return position + stepSize * windVector;
}

QVector3D streamLineMapper::KuttaSecondOrderIntegration(QVector3D position, float stepSize) {

    QVector3D deltaX = stepSize * interpolateWindVector(position);
    QVector3D mid = interpolateWindVector(position + (deltaX / 2));
    return position + stepSize * mid;
}

QVector3D streamLineMapper::KuttaFourthOrderIntegration(QVector3D position, float stepSize) {
    QVector3D k1,k2,k3,k4;
    QVector3D k2v, k3v, k4v;
    k1 = stepSize * interpolateWindVector(position);
    k2v = interpolateWindVector(position + (k1 / 2));
    k2 = stepSize * k2v;
    k3v = interpolateWindVector(position + (k2/2));
    k3 = stepSize * k3v;
    k4v = interpolateWindVector(position+k3);
    k4 = stepSize * k4v;
    return position + (k1/6) + (k2/3) + (k3/3) + (k4/6);
}

QVector3D streamLineMapper::interpolateWindVector(QVector3D position) {
    QVector<PointWind> cell = findCell(position);
    PointWind a = singleInterpolation(singleInterpolation(cell.at(0),cell.at(2),position.z(),Z), singleInterpolation(cell.at(1),cell.at(3),position.z(),Z),position.x(), X);
    PointWind b = singleInterpolation(singleInterpolation(cell.at(4),cell.at(6),position.z(),Z), singleInterpolation(cell.at(5),cell.at(7),position.z(),Z),position.x(), X);
    PointWind c = singleInterpolation(a,b,position.y(),Y);
    return QVector3D(c.vx, c.vy,c.vz);
}

QVector<PointWind> streamLineMapper::findCell(QVector3D position) {
    if(position.x() >= 31) position.setX(30);
    if(position.y() >= 31) position.setY(30);
    if(position.z() >= 31) position.setZ(30);
    QVector<PointWind> temp;
    int x0,x1,y0,y1,z0,z1;
    x0 = int(position.x());
    x1 = x0+1;
    y0 = int(position.y());
    y1 = y0+1;
    z0 = int(position.z());
    z1 = z0+1;
    PointWind temp2[8] = {
            PointWind(x0, y0, z0, source->getDataValue(z0, y0, x0, 0), source->getDataValue(z0, y0, x0, 1), source->getDataValue(z0, y0, x0, 2)),
            PointWind(x1, y0, z0, source->getDataValue(z0, y0, x1, 0), source->getDataValue(z0, y0, x1, 1), source->getDataValue(z0, y0, x1, 2)),
            PointWind(x0, y0, z1, source->getDataValue(z1, y0, x0, 0), source->getDataValue(z1, y0, x0, 1), source->getDataValue(z1, y0, x0, 2)),
            PointWind(x1, y0, z1, source->getDataValue(z1, y0, x1, 0), source->getDataValue(z1, y0, x1, 1), source->getDataValue(z1, y0, x1, 2)),

            PointWind(x0, y1, z0, source->getDataValue(z0, y1, x0, 0), source->getDataValue(z0, y1, x0, 1), source->getDataValue(z0, y1, x0, 2)),
            PointWind(x1, y1, z0, source->getDataValue(z0, y1, x1, 0), source->getDataValue(z0, y1, x1, 1), source->getDataValue(z0, y1, x1, 2)),
            PointWind(x0, y1, z1, source->getDataValue(z1, y1, x0, 0), source->getDataValue(z1, y1, x0, 1), source->getDataValue(z1, y1, x0, 2)),
            PointWind(x1, y1, z1, source->getDataValue(z1, y1, x1, 0), source->getDataValue(z1, y1, x1, 1), source->getDataValue(z1, y1, x1, 2))
    };
    for(int i = 0; i < 8; i++)
    {
        temp.append(temp2[i]);
    }
    return temp;
}

PointWind streamLineMapper::singleInterpolation(PointWind p1, PointWind p2, float point, int component) {
    float x1;
    float x2;
    QVector3D y1;
    QVector3D y2;
    QVector3D v;
    switch(component) {
        case X:
            x1 = p1.x / 31;
            x2 = p2.x / 31;
            y1 = QVector3D(p1.vx,p1.vy,p1.vz);
            y2 = QVector3D(p2.vx,p2.vy,p2.vz);
            v = y1 + (point/31 - x1) * (y2 - y1) / (x2 - x1);
            return {point, p1.y, p1.z, v.x(),v.y(),v.z()};
        case Y:
            x1 = p1.y / 31;
            x2 = p2.y / 31;
            y1 = QVector3D(p1.vx,p1.vy,p1.vz);
            y2 = QVector3D(p2.vx,p2.vy,p2.vz);
            v = y1 + (point/31 - x1) * (y2 - y1) / (x2 - x1);
            return {p1.x, point, p1.z, v.x(),v.y(),v.z()};
        case Z:
            x1 = p1.z / 31;
            x2 = p2.z / 31;
            y1 = QVector3D(p1.vx,p1.vy,p1.vz);
            y2 = QVector3D(p2.vx,p2.vy,p2.vz);
            v = y1 + (point/31 - x1) * (y2 - y1) / (x2 - x1);
            return {p1.x, p1.y, point, v.x(),v.y(),v.z()};
    }
}
