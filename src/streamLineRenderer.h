//
// Created by Paul on 10.07.2022.
//

#ifndef DV_STREAMLINERENDERER_H
#define DV_STREAMLINERENDERER_H


#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "streamLineMapper.h"
#include <QOpenGLFunctions>
#include <iostream>

class streamLineRenderer {
public:
    explicit streamLineRenderer(streamLineMapper *mapper);
    virtual ~streamLineRenderer();
    // Draw the bounding box to the current OpenGL viewport.
    void drawStreamLines(QMatrix4x4 mvpMatrix, int amount, float stepSize, int time, int method);
    void updateStreamLines(int amount, float stepSize, int time, int method);
    void setMapper(streamLineMapper *mapper);
private:
    streamLineRenderer();
    void initOpenGLShaders();
    void initVertexBuffer();
    streamLineMapper *mapper;
    QVector<QVector3D> streamLines;
    QVector<QVector3D> seedPoints;
    int currentAmount;
    float currentStepSize;
    int currentTime;
    int currentMethod;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLBuffer vertexBuffer;
    QOpenGLVertexArrayObject vertexArrayObject;
};


#endif //DV_STREAMLINERENDERER_H
