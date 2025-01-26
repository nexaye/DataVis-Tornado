//
// Created by Paul on 06.06.2022.
//


#include <iostream>
#include "HorizontalSliceToContourLineMapper.h"

HorizontalSliceToContourLineMapper::HorizontalSliceToContourLineMapper(): source(nullptr) {

}

HorizontalSliceToContourLineMapper::HorizontalSliceToContourLineMapper(FlowDataSource *source): HorizontalSliceToContourLineMapper() {
    setDataSource(source);
}

void HorizontalSliceToContourLineMapper::setDataSource(FlowDataSource *source) {
    this->source = source;
}

QVector<QVector3D> HorizontalSliceToContourLineMapper::mapSliceToContourLineSegments(int slice, int component, float isovalue) {
    QVector<QVector3D> vector;
    for(int y = 0; y < 31; y++)
    {
        for(int x = 0; x <31; x++){

            Point temp[4] =
            {
                    Point(x,y,slice,source->getDataValue(slice, y, x, component)), //v3
                    Point(x+1,y,slice,source->getDataValue(slice, y, x+1, component)), //v2
                    Point(x+1,y+1,slice,source->getDataValue(slice, y+1, x+1, component)), //v1
                    Point(x,y+1,slice,source->getDataValue(slice, y+1, x, component)) //v0
            };
            uint8_t bits = 0b0000;
            if(temp[3].value >= isovalue) bits = bits | 0b0001;
            if(temp[2].value >= isovalue) bits = bits | 0b0010;
            if(temp[1].value >= isovalue) bits = bits | 0b0100;
            if(temp[0].value >= isovalue) bits = bits | 0b1000;
            if((bits & 0b1000) > 0){
                bits = bits ^ 0b1111;
            }
            switch(bits)
            {
                case 1:
                    vector.append(QVector3D(float(x)/31, interpolate(temp[3],temp[0],isovalue,true),float(slice)/31));
                    vector.append(QVector3D(interpolate(temp[3],temp[2],isovalue,false) , float(y+1)/31 ,float(slice)/31));
                    break;
                case 2:
                    vector.append(QVector3D(float(x+1)/31, interpolate(temp[2],temp[1],isovalue,true),float(slice)/31));
                    vector.append(QVector3D(interpolate(temp[2],temp[3],isovalue,false) , float(y+1)/31 ,float(slice)/31));
                    break;
                case 3:
                    vector.append(QVector3D(float(x)/31, interpolate(temp[3],temp[0],isovalue,true),float(slice)/31));
                    vector.append(QVector3D(float(x+1)/31, interpolate(temp[2],temp[1],isovalue,true),float(slice)/31));
                    break;
                case 4:
                    vector.append(QVector3D(float(x+1)/31, interpolate(temp[1],temp[2],isovalue,true),float(slice)/31));
                    vector.append(QVector3D(interpolate(temp[1],temp[0],isovalue,false) , float(y)/31 ,float(slice)/31));
                    break;
                case 5:
                    if(interpolateMidPoint(temp) >= isovalue){
                        vector.append(QVector3D(float(x)/31, interpolate(temp[3],temp[0],isovalue,true),float(slice)/31));
                        vector.append(QVector3D(interpolate(temp[3],temp[2],isovalue,false) , float(y+1)/31 ,float(slice)/31));
                        vector.append(QVector3D(float(x+1)/31, interpolate(temp[1],temp[2],isovalue,true),float(slice)/31));
                        vector.append(QVector3D(interpolate(temp[1],temp[0],isovalue,false) , float(y)/31 ,float(slice)/31));
                    }
                    else{
                        vector.append(QVector3D(float(x)/31, interpolate(temp[3],temp[0],isovalue,true),float(slice)/31));
                        vector.append(QVector3D(interpolate(temp[1],temp[0],isovalue,false) , float(y)/31 ,float(slice)/31));
                        vector.append(QVector3D(float(x+1)/31, interpolate(temp[2],temp[1],isovalue,true),float(slice)/31));
                        vector.append(QVector3D(interpolate(temp[2],temp[3],isovalue,false) , float(y+1)/31 ,float(slice)/31));
                    }
                    break;
                case 6:
                    vector.append(QVector3D(interpolate(temp[1],temp[0],isovalue,false) , float(y)/31 ,float(slice)/31));
                    vector.append(QVector3D(interpolate(temp[2],temp[3],isovalue,false) , float(y+1)/31 ,float(slice)/31));
                    break;
                case 7:
                    vector.append(QVector3D(float(x)/31, interpolate(temp[3],temp[0],isovalue,true),float(slice)/31));
                    vector.append(QVector3D(interpolate(temp[1],temp[0],isovalue,false) , float(y)/31 ,float(slice)/31));
                    break;
                default:
                    break;
            }
        }
    }
    return vector;
}

float HorizontalSliceToContourLineMapper::interpolate(Point p1, Point p2, float isovalue, bool useY) {
    float maxsteps = 31.0f;
    float x1;
    float x2;
    float y1 = float(p1.value);
    float y2 = float(p2.value);
    if(useY){
        x1 = float(p1.y)/maxsteps;
        x2 = float(p2.y)/maxsteps;
    }else{
        x1 = float(p1.x)/maxsteps;
        x2 = float(p2.x)/maxsteps;
    }
    return ((-x1*y2)+(x1*isovalue)+(x2 * y1) - (x2 * isovalue)) / (y1 - y2);
}

float HorizontalSliceToContourLineMapper::interpolateMidPoint(Point *points) {
    Point midPoint = Point(1/4*(points[0].x+points[1].x+points[2].x+points[3].x),1/4*(points[0].y+points[1].y+points[2].y+points[3].y),0,0);
    Point a = Point(points[3].x, midPoint.y, 0, interpolateX(points[0],points[3], midPoint.y, true));
    Point b = Point(points[1].x, midPoint.y, 0, interpolateX(points[1],points[2], midPoint.y, true));
    return interpolateX(a,b,midPoint.x,false);
}

float HorizontalSliceToContourLineMapper::interpolateX(Point p1, Point p2, float point, bool useY) {
    float x1;
    float x2;
    float y1;
    float y2;
    float v;
    if(useY) {
        x1 = p1.y / 31;
        x2 = p2.y / 31;
        y1 = p1.value;
        y2 = p2.value;
        v = y1 + (point / 31 - x1) * (y2 - y1) / (x2 - x1);
        return v;
    }
    else {
        x1 = p1.x / 31;
        x2 = p2.x / 31;
        y1 = p1.value;
        y2 = p2.value;
        v = y1 + (point / 31 - x1) * (y2 - y1) / (x2 - x1);
        return v;
    }
}
