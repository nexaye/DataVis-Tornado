//
// Created by Paul on 29.05.2022.
//

#ifndef DV_HORIZONTALSLICERENDERER_H
#define DV_HORIZONTALSLICERENDERER_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "HorizontalSliceToImageMapper.h"
#include "Util.h"
class HorizontalSliceRenderer {
public:
    explicit HorizontalSliceRenderer(HorizontalSliceToImageMapper *mapper);
    virtual ~HorizontalSliceRenderer();
    // Draw the bounding box to the current OpenGL viewport.
    void drawHorizontalSlice(QMatrix4x4 mvpMatrix, float currentSlice, int component);
    void drawOutlineSlice(QMatrix4x4 mvpMatrix, float currentSlice);
    void setMapper(HorizontalSliceToImageMapper *mapper);
private:
    HorizontalSliceRenderer();
    void initOpenGLShaders();
    void initHorizontalSliceGeometry();
    HorizontalSliceToImageMapper *mapper;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLShaderProgram shaderProgram2;
    QOpenGLBuffer vertexBuffer;
    QOpenGLVertexArrayObject vertexArrayObject;
};


#endif //DV_HORIZONTALSLICERENDERER_H
