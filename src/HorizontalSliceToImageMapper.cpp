//
// Created by Paul on 29.05.2022.
//

#include <iostream>
#include <cmath>
#include "HorizontalSliceToImageMapper.h"

HorizontalSliceToImageMapper::HorizontalSliceToImageMapper(): source(nullptr){

}

HorizontalSliceToImageMapper::HorizontalSliceToImageMapper(FlowDataSource *source): HorizontalSliceToImageMapper() {
    setDataSource(source);
}
void HorizontalSliceToImageMapper::setImageSource(QString path) {
    this->imagePath = path;
}

QImage HorizontalSliceToImageMapper::readImage() {
    return QImage(imagePath).mirrored(true, true);
}

void HorizontalSliceToImageMapper::setDataSource(FlowDataSource *source) {
    this->source = source;
}

QImage HorizontalSliceToImageMapper::mapSliceToImage(int slice, int component) {
    if(source != nullptr)
    {
        QImage image = QImage(32, 32, QImage::Format_RGB888);
        for(int x = 0; x < 32; x++){
            for(int y = 0; y < 32; y++)
            {
                float temp;
                float max = 0.3;
                float min = -0.3;
                int index;
                if(component == MAG){
                    temp = source->getAbsoluteValue(slice, y, x);
                    if(temp > 0.3) temp = 0.3;
                    if(temp < -0.3) temp -0.3;
                }
                else{
                    temp = source->getDataValue(slice, y, x, component);
                    if(temp > 0.3) temp = 0.3;
                    if(temp < -0.3) temp -0.3;
                }
                index = (int((temp-min) / (max-min)*99));
                int r= qRed(colorMap[index]);
                int g=  qGreen(colorMap[index]);
                int b=  qBlue(colorMap[index]);
                int rgb = (r+g+b) / 3;
                image.setPixel(x,y, colorMap2[index]);
            }
        }
        return image.mirrored(false, false);
    }
    return readImage();
}
