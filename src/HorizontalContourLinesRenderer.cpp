//
// Created by Paul on 06.06.2022.
//

#include <QOpenGLFunctions>
#include "HorizontalContourLinesRenderer.h"
HorizontalContourLinesRenderer::HorizontalContourLinesRenderer(): vertexBuffer(QOpenGLBuffer::VertexBuffer), mapper(nullptr){
    initOpenGLShaders();
    initHorizontalSliceGeometry();
}

HorizontalContourLinesRenderer::HorizontalContourLinesRenderer(HorizontalSliceToContourLineMapper *mapper): HorizontalContourLinesRenderer() {
    setMapper(mapper);
}

HorizontalContourLinesRenderer::~HorizontalContourLinesRenderer() {
    vertexBuffer.destroy();
}

void HorizontalContourLinesRenderer::drawHorizontalSliceContour(QMatrix4x4 mvpMatrix, float currentSlice, int component, float isovalue, bool selected) {
    shaderProgram.link();
    // Tell OpenGL to use the shader program of this class.
    shaderProgram.bind();
    vertexBuffer.bind();
    QVector<QVector3D> temp = mapper->mapSliceToContourLineSegments(currentSlice,component,isovalue);
    vertexBuffer.allocate(temp.data(), temp.size()*3*sizeof(float));
    vertexBuffer.release();
    // Bind the vertex array object that links to the bounding box vertices.
    vertexArrayObject.bind();
    // Set the model-view-projection matrix as a uniform value.
    shaderProgram.setUniformValue("mvpMatrix", mvpMatrix);
    shaderProgram.setUniformValue("selected", selected);
    shaderProgram.setUniformValue("currentSlice", currentSlice/31);
    // Issue OpenGL draw commands.
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glLineWidth(2);
    f->glDrawArrays(GL_LINES, 0, temp.size());
    // Release objects until next render cycle.
    vertexArrayObject.release();
    shaderProgram.release();
}

void HorizontalContourLinesRenderer::setMapper(HorizontalSliceToContourLineMapper *mapper) {
    this->mapper = mapper;
}

void HorizontalContourLinesRenderer::initOpenGLShaders() {
    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, SHADER_DIR + QString("contour_vshader.glsl")))
    {
        std::cout << "Vertex shader error:\n" << shaderProgram.log().toStdString() << "\n" << std::flush;
        return;
    }
    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, SHADER_DIR + QString("contour_fshader.glsl")))
    {
        std::cout << "Fragment shader error:\n" << shaderProgram.log().toStdString() << "\n" << std::flush;
        return;
    }

    if (!shaderProgram.link())
    {
        std::cout << "Shader link error:\n" << shaderProgram.log().toStdString() << "\n" << std::flush;
        return;
    }
}

void HorizontalContourLinesRenderer::initHorizontalSliceGeometry() {
    // Create vertex buffer and upload vertex data to buffer.
    vertexBuffer.create(); // make sure to destroy in destructor!
    QOpenGLVertexArrayObject::Binder vaoBinder(&vertexArrayObject);
    if (vertexArrayObject.isCreated())
    {
        vertexBuffer.bind();
        shaderProgram.setAttributeBuffer("vertexPosition", GL_FLOAT, 0, 3, 3*sizeof(float));
        shaderProgram.enableAttributeArray("vertexPosition");
        vertexBuffer.release();
    }
}
