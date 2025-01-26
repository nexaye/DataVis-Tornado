//
// Created by Paul on 29.05.2022.
//

#include <iostream>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include "HorizontalSliceRenderer.h"

HorizontalSliceRenderer::HorizontalSliceRenderer(HorizontalSliceToImageMapper *mapper): HorizontalSliceRenderer(){
    setMapper(mapper);
}

HorizontalSliceRenderer::HorizontalSliceRenderer(): vertexBuffer(QOpenGLBuffer::VertexBuffer), mapper(nullptr){
    initOpenGLShaders();
    initHorizontalSliceGeometry();
}

HorizontalSliceRenderer::~HorizontalSliceRenderer() {
    vertexBuffer.destroy();
}

void HorizontalSliceRenderer::drawHorizontalSlice(QMatrix4x4 mvpMatrix, float currentSlice, int component) {
    shaderProgram.link();
    // Tell OpenGL to use the shader program of this class.
    shaderProgram.bind();

    // Bind the vertex array object that links to the bounding box vertices.
    vertexArrayObject.bind();
    //Set texture
    QImage img = mapper->mapSliceToImage((int)currentSlice, component);
    QOpenGLTexture texture(QOpenGLTexture::Target2D);
    texture.create();
    texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    texture.setData(img);
    texture.setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
    const int textureUnit = 0; // select a texture unit
    texture.bind(textureUnit);
    // Set the model-view-projection matrix as a uniform value.
    shaderProgram.setUniformValue("mvpMatrix", mvpMatrix);
    shaderProgram.setUniformValue("currentSlice", currentSlice/31);
    shaderProgram.setUniformValue("colorMappingTexture", textureUnit);
    // Issue OpenGL draw commands.
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glLineWidth(2);
    f->glDrawArrays(GL_TRIANGLES, 0, 6);
    // Release objects until next render cycle.
    vertexArrayObject.release();
    shaderProgram.release();
    texture.destroy();
}

void HorizontalSliceRenderer::drawOutlineSlice(QMatrix4x4 mvpMatrix, float currentSlice) {
        shaderProgram2.link();
        // Tell OpenGL to use the shader program of this class.
        shaderProgram2.bind();
        // Bind the vertex array object that links to the bounding box vertices.
        vertexArrayObject.bind();
        // Set the model-view-projection matrix as a uniform value.
        shaderProgram2.setUniformValue("mvpMatrix", mvpMatrix);
        shaderProgram2.setUniformValue("currentSlice", currentSlice/31);
        // Issue OpenGL draw commands.
        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glLineWidth(2);
        f->glDrawArrays(GL_LINE_STRIP, 6, 5);
        // Release objects until next render cycle.
        vertexArrayObject.release();
        shaderProgram2.release();
}
void HorizontalSliceRenderer::setMapper(HorizontalSliceToImageMapper *mapper) {
    this->mapper = mapper;
}

void HorizontalSliceRenderer::initOpenGLShaders() {
    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
        SHADER_DIR + QString("horizontal_vshader.glsl")) || !shaderProgram2.addShaderFromSourceFile(QOpenGLShader::Vertex,
            SHADER_DIR + QString("outline_vshader.glsl")))
    {
        std::cout << "Vertex shader error:\n"
                  << shaderProgram.log().toStdString() << "\n" << std::flush;
        return;
    }

    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
        SHADER_DIR + QString("horizontal_fshader.glsl")) || !shaderProgram2.addShaderFromSourceFile(QOpenGLShader::Fragment,
            SHADER_DIR + QString("outline_fshader.glsl")))
    {
        std::cout << "Fragment shader error:\n"
                  << shaderProgram.log().toStdString() << "\n" << std::flush;
        return;
    }

    if (!shaderProgram.link() || !shaderProgram2.link())
    {
        std::cout << "Shader link error:\n"
                  << shaderProgram.log().toStdString() << "\n" << std::flush;
        return;
    }
}

void HorizontalSliceRenderer::initHorizontalSliceGeometry() {
    // Vertices of a unit cube that represents the bounding box.
    float sliceAndOutlineVertices[11][3] = {
            {0, 0, 1}, // Actual slice vertices
            {0, 1, 1},
            {1, 0, 1},
            {0, 1, 1},
            {1, 1, 1},
            {1, 0, 1},
            {0, 0, 1}, // Outline
            {0, 1, 1},
            {1, 1, 1},
            {1, 0, 1},
            {0, 0, 1},
    };
    // Create vertex buffer and upload vertex data to buffer.
    vertexBuffer.create(); // make sure to destroy in destructor!
    vertexBuffer.bind();
    vertexBuffer.allocate(sliceAndOutlineVertices, 11 * 3 * sizeof(float));
    vertexBuffer.release();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vertexArrayObject);
    if (vertexArrayObject.isCreated())
    {
        vertexBuffer.bind();
        shaderProgram.setAttributeBuffer("vertexPosition", GL_FLOAT, 0, 3, 3*sizeof(float));
        shaderProgram.enableAttributeArray("vertexPosition");
        shaderProgram2.setAttributeBuffer("vertexPosition", GL_FLOAT, 0, 3, 3*sizeof(float));
        shaderProgram2.enableAttributeArray("vertexPosition");
        vertexBuffer.release();
    }
}
