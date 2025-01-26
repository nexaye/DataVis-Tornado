//
// Created by Paul on 10.07.2022.
//
#include <random>
#include "streamLineRenderer.h"

streamLineRenderer::streamLineRenderer(streamLineMapper *mapper) : streamLineRenderer(){
    setMapper(mapper);
}
streamLineRenderer::streamLineRenderer() {
    initOpenGLShaders();
    initVertexBuffer();
    currentTime = -1;
    currentStepSize = -1;
    currentAmount = 0;
    currentMethod = -1;

}
streamLineRenderer::~streamLineRenderer() {
    vertexBuffer.destroy();
}

void streamLineRenderer::drawStreamLines(QMatrix4x4 mvpMatrix, int amount, float stepSize, int time, int method) {
    shaderProgram.link();
    // Tell OpenGL to use the shader program of this class.
    shaderProgram.bind();
    vertexBuffer.bind();
    updateStreamLines(amount, stepSize, time, method);
    vertexBuffer.allocate(streamLines.data(), streamLines.size()*3*sizeof(float));
    vertexBuffer.release();
    // Bind the vertex array object that links to the bounding box vertices.
    vertexArrayObject.bind();
    // Set the model-view-projection matrix as a uniform value.
    shaderProgram.setUniformValue("mvpMatrix", mvpMatrix);
    // Issue OpenGL draw commands.
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glLineWidth(2);
    f->glDrawArrays(GL_LINES, 0, streamLines.size());
    // Release objects until next render cycle.
    vertexArrayObject.release();
    shaderProgram.release();
}

void streamLineRenderer::updateStreamLines(int amount, float stepSize, int time, int method) {
    if(!(currentAmount != amount || currentStepSize != stepSize || currentTime != time || currentMethod != method)) return;
    int difference = amount - currentAmount;
    currentAmount = amount;
    currentStepSize = stepSize;
    currentTime = time;
    currentMethod = method;
    streamLines.clear();
    //compute seedPoints
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(15.5, 15.5);
    std::uniform_real_distribution<> dist2(0, 31);
    if(difference > 0)
    {
        for(int i = 0; i< difference; i++)
        {
            seedPoints.append(QVector3D(dist(gen),dist(gen),dist2(gen)));
        }
    }
    if(difference < 0)
    {
        difference *= -1;
        for(int i = 0; i < difference; i++)
        {
            seedPoints.takeLast();
        }
    }
    //compute seedPoints
    this->streamLines.append(mapper->computeStreamLines(seedPoints, method, currentStepSize));
}

void streamLineRenderer::setMapper(streamLineMapper *mapper) {
    this->mapper = mapper;
}

void streamLineRenderer::initOpenGLShaders() {
    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, SHADER_DIR + QString("stream_vshader.glsl")))
    {
        std::cout << "Vertex shader error:\n" << shaderProgram.log().toStdString() << "\n" << std::flush;
        return;
    }
    if (!shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, SHADER_DIR + QString("stream_fshader.glsl")))
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

void streamLineRenderer::initVertexBuffer(){
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
