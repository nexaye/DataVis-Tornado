//
// Created by Paul on 06.06.2022.
//

#ifndef DV_HORIZONTALCONTOURLINESRENDERER_H
#define DV_HORIZONTALCONTOURLINESRENDERER_H


#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "HorizontalSliceToContourLineMapper.h"
#include <iostream>

class HorizontalContourLinesRenderer {
public:
    explicit HorizontalContourLinesRenderer(HorizontalSliceToContourLineMapper *mapper);
    virtual ~HorizontalContourLinesRenderer();
    // Draw the bounding box to the current OpenGL viewport.
    void drawHorizontalSliceContour(QMatrix4x4 mvpMatrix, float currentSlice, int component, float isovalue, bool selected);
    void setMapper(HorizontalSliceToContourLineMapper *mapper);
private:
    HorizontalContourLinesRenderer();
    void initOpenGLShaders();
    void initHorizontalSliceGeometry();
    HorizontalSliceToContourLineMapper *mapper;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLShaderProgram shaderProgram2;
    QOpenGLBuffer vertexBuffer;
    QOpenGLVertexArrayObject vertexArrayObject;
};


#endif //DV_HORIZONTALCONTOURLINESRENDERER_H
