#include "opengldisplaywidget.h"

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <iostream>
#include <QPainter>
#include "math.h"


OpenGLDisplayWidget::OpenGLDisplayWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      distanceToCamera(-8.0)
{
    setFocusPolicy(Qt::StrongFocus);
    timerActive = false;
}

void OpenGLDisplayWidget::timerEvent(QTimerEvent *event) {
    time++;
    update();
}

OpenGLDisplayWidget::~OpenGLDisplayWidget()
{
    // Clean up visualization pipeline.
    delete bboxRenderer;
    delete hSliceRenderer;
    delete hSliceMapper;
    delete dataSource;
    delete painter;
    delete hContourMapper;
    delete hContourRenderer;
    delete sLineRenderer;
    delete sLineMapper;
}


QString OpenGLDisplayWidget::openGLString()
{
    QString profileStr;
    switch (format().profile())
    {
    case QSurfaceFormat::NoProfile:
        profileStr = "no profile";
        break;
    case QSurfaceFormat::CompatibilityProfile:
        profileStr = "compatibility profile";
        break;
    case QSurfaceFormat::CoreProfile:
        profileStr = "core profile";
        break;
    }

    return QString("%1.%2 (%3)").arg(format().majorVersion())
            .arg(format().minorVersion()).arg(profileStr);
}


void OpenGLDisplayWidget::initializeGL()
{
    // Query and display some information about the used OpenGL context.
    std::cout << "Initializing OpenGLDisplayWidget with OpenGL version "
              <<  openGLString().toStdString() << ".\n" << std::flush;

    // Set the backgound color of the OpenGL display enable the depth buffer.
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0, 0, 0, 1);
    f->glEnable(GL_DEPTH_TEST);
    // Our own initialization of the visualization pipeline.
    initVisualizationPipeline();
}


void OpenGLDisplayWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio of the current viewport.
    float aspectRatio = float(w) / std::max(1, h);

    // Reset projection and set new perspective projection.
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0, aspectRatio, 0.05, 25.0);

    // Update model-view-projection matrix with new projection.
    updateMVPMatrix();
}


void OpenGLDisplayWidget::paintGL()
{
    // Clear color and depth buffer.
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(time != dataSource->getTime())
    {
        dataSource->createData(time);
    }
    painter->begin(this);
    // Call renderer modules.
    f->glEnable(GL_DEPTH_TEST);
    bboxRenderer->drawBoundingBox(mvpMatrix);
    hSliceRenderer->drawHorizontalSlice(mvpMatrix,(float)slice, component);
    if(state.sLine) sLineRenderer->drawStreamLines(mvpMatrix,state.streamLineAmount,state.stepSize,this->time,state.method);
    f->glDisable(GL_DEPTH_TEST);
    hSliceRenderer->drawOutlineSlice(mvpMatrix, (float)slice);
    for(int i = 0; i<state.isoLines.size();i++)
    {
        bool temp = false;
        if(i == state.currentlySelectedIsoLine) temp = true;
        hContourRenderer->drawHorizontalSliceContour(mvpMatrix,(float)slice,component,state.isoLines.at(i),temp);
    }
    // ....
    drawUI();
}

void OpenGLDisplayWidget::drawUI()
{
    painter->setFont(QFont("Arial",14));
    painter->setPen(Qt::white);
    painter->drawText(5, 30, "State");
    painter->drawText(5, 50, "--------------------");
    painter->drawText(5,70,"Slice: " + QString::number(slice+1));
    QString temp = state.comp[component];
    painter->drawText(5,100,"Component: " + temp);
    painter->drawText(5,130,"Time: " + QString::number(this->time));
    for(int i =0; i < state.isoLines.size();i++)
    {
        if(i == state.currentlySelectedIsoLine) painter->setPen(Qt::red);
        painter->drawText(5, 160+30*i,"Iso" + QString::number(i) + ": " + QString::number(state.isoLines.at(i)));
        painter->setPen(Qt::white);
    }
    painter->drawText(5, 310, state.sLine ? "sLine state: On" : "sLine state: Off");
    if(state.sLine || state.editMode){
        painter->setPen(Qt::red);
        switch(state.currentlySelectedSLOption)
        {
            case(0):
                painter->drawText(5,340, "sLine amount: " + QString::number(state.streamLineAmount));
                painter->setPen(Qt::white);
                painter->drawText(5,370, "sLine stepSize: " + QString::number(state.stepSize));
                painter->drawText(5,400, "sLine method: " + state.meth[state.method]);
                break;
            case(1):
                painter->drawText(5,370, "sLine stepSize: " + QString::number(state.stepSize));
                painter->setPen(Qt::white);
                painter->drawText(5,340, "sLine amount: " + QString::number(state.streamLineAmount));
                painter->drawText(5,400, "sLine method: " + state.meth[state.method]);
                break;
            case(2):
                painter->drawText(5,400, "sLine method: " + state.meth[state.method]);
                painter->setPen(Qt::white);
                painter->drawText(5,340, "sLine amount: " + QString::number(state.streamLineAmount));
                painter->drawText(5,370, "sLine stepSize: " + QString::number(state.stepSize));
                break;
            default:
                painter->setPen(Qt::white);
                painter->drawText(5,400, "sLine method: " + state.meth[state.method]);
                painter->drawText(5,340, "sLine amount: " + QString::number(state.streamLineAmount));
                painter->drawText(5,370, "sLine stepSize: " + QString::number(state.stepSize));

        }
    }
    if(state.editMode)
    {
        painter->setPen(Qt::red);
        switch(state.editModes)
        {
            case IS:
                painter->drawText(5, 440, "Controls: Edit (IsoLines)");
                painter->setPen(Qt::white);
                painter->drawText(5, 690,  u8"\u2B9D: Select previous isoline");
                painter->drawText(5, 720,  u8"\u2B9F: Select next isoline");
                painter->drawText(5, 750,  u8"\u2B9E: Increase isovalue");
                painter->drawText(5, 780,  u8"\u2B9C: Decrease isovalue");
                painter->drawText(5, 810, "D: Add isoline");
                painter->drawText(5, 840, "F: Remove selected isoLine");
                painter->drawText(5, 870, "X: Edit streamLines");
                break;
            case SL:
                painter->drawText(5, 440, "Controls: Edit (StreamLines)");
                painter->setPen(Qt::white);
                painter->drawText(5, 690, u8"\u2B9D: Previous option");
                painter->drawText(5, 720, u8"\u2B9F: Next option");
                painter->drawText(5, 750, u8"\u2B9E: Increase value");
                painter->drawText(5, 780, u8"\u2B9C: Decrease value");
                painter->drawText(5, 810, "D: Add isoline");
                painter->drawText(5, 840, "F: Remove selected isoLine");
                painter->drawText(5, 870, "X: Edit IsoLines");
                break;
        }
        painter->setPen(Qt::white);

    }
    else {
        painter->drawText(5, 440,"Controls: Normal");
        painter->drawText(5, 690, u8"\u2B9D: Move slice up");
        painter->drawText(5, 720, u8"\u2B9F: Move slice down");
        painter->drawText(5, 750, u8"\u2B9E: Time step +1");
        painter->drawText(5, 780, u8"\u2B9C: Time step -1");
    }
    painter->drawText(5, 460, "--------------------");
    painter->drawText(5, 480, "Q: Show X values on slice");
    painter->drawText(5, 510, "W: Show Y values on slice");
    painter->drawText(5, 540, "E: Show Z values on slice");
    painter->drawText(5, 570, "R: Show MAG values on slice");
    painter->drawText(5, 600, "S: Start Animation");
    painter->drawText(5, 630, "T: Toggle edit mode");
    painter->drawText(5, 660, "Y: Toggle sLine");
    int mapSize = 100;
    int recWidth = 8;
    for(int i = 0; i<100;i++)
    {
        painter->fillRect((window()->width()/2)-(50*8)+i*8, window()->height()-50,8,50,QColor(colorMap2[i]));
    }
    painter->setFont(QFont("Arial",9));
    painter->drawLine((window()->width()/2)-(50*8)+4, window()->height()-55, (window()->width()/2)-(50*8)+4, window()->height()-65); //left line
    painter->drawLine((window()->width()/2)-(50*8)+100*8 - 4, window()->height()-55, (window()->width()/2)-(20*20)+100*8 - 4, window()->height()-65); //right line
    painter->drawLine((window()->width()/2)-(50*8)+50*8, window()->height()-55, (window()->width()/2)-(20*20)+20*20, window()->height()-65); //middle line

    painter->drawText((window()->width()/2)-(50*8)+50*8 - 4,window()->height()-70, QString::number(0));
    painter->drawText((window()->width()/2)-(50*8)+0*50- 8,window()->height()-70, QString::number(-0.3));
    painter->drawText((window()->width()/2)-(50*8)+100*8 - 16,window()->height()-70, QString::number(0.3));
    painter->end();
}

void OpenGLDisplayWidget::mousePressEvent(QMouseEvent *e)
{
    // Save the current position of the mouse pointer for subsequent use
    // in mouseMoveEvent().
    lastMousePosition = QVector2D(e->localPos());
}


void OpenGLDisplayWidget::mouseMoveEvent(QMouseEvent *e)
{
    // If the user holds the left mouse button while moving the mouse, update
    // the rotation angles that specify from which side the grid visualization
    // is viewed.
    if (e->buttons() & Qt::LeftButton)
    {
        // Vector that points from the last stored position of the mouse
        // pointer to the current position.
        QVector2D mousePosDifference = QVector2D(e->localPos()) - lastMousePosition;

        // Update rotation angles in x and y direction. The factor "10" is an
        // arbitrary scaling constant that controls the sensitivity of the
        // mouse.
        rotationAngles.setX(
                    fmod(rotationAngles.x() + mousePosDifference.x()/10.,
                         360.));
        rotationAngles.setY(
                    fmod(rotationAngles.y() + mousePosDifference.y()/10.,
                         360.));

        // Store current position of mouse pointer for next call to this method.
        lastMousePosition = QVector2D(e->localPos());

        // Update model-view-projection matrix with new rotation angles.
        updateMVPMatrix();

        // Redraw OpenGL.
        update();
    }
}


void OpenGLDisplayWidget::wheelEvent(QWheelEvent *e)
{
    // Update distance of the camera to the rendered visualization. The factor
    // "500" is arbitrary and controls that sensitivity of the mouse.
    distanceToCamera += e->angleDelta().y() / 500.0;

    // Update model-view-projection matrix with new distance to camera.
    updateMVPMatrix();

    // Redraw OpenGL.
    update();
}


void OpenGLDisplayWidget::keyPressEvent(QKeyEvent *e)
{

    switch(e->key())
    {
        case Qt::Key_Up:
            if(state.editMode)
                switch(state.editModes){
                    case IS:
                        state.selectNextIsoLine(-1);
                        break;
                    case SL:
                        state.changeOption(-1);
                }
            else moveSliceForward();
            break;
        case Qt::Key_Down:
            if(state.editMode)
            switch(state.editModes){
                case IS:
                    state.selectNextIsoLine(1);
                    break;
                case SL:
                    state.changeOption(1);
            }
            else moveSliceBackward();
            break;
        case Qt::Key_Right:
            if(state.editMode) {
                switch(state.editModes){
                    case IS:
                        state.editIsoLineValue(0.01);
                        break;
                    case SL:
                        state.doThing(1,0.1,1);
                        break;
                }
            }
            else changeTime(1);
            break;
        case Qt::Key_Left:
            if(state.editMode) {
                switch(state.editModes){
                    case IS:
                        state.editIsoLineValue(-0.01);
                        break;
                    case SL:
                        state.doThing(-1,-0.1,-1);
                        break;
                }
            }
            else
            if(changeTime(-1)) return;
            break;
        case Qt::Key_Q:
            component = X;
            break;
        case Qt::Key_W:
            component = Y;
            break;
        case Qt::Key_E:
            component = Z;
            break;
        case Qt::Key_R:
            component = MAG;
            break;
        case Qt::Key_S:
            if(timerActive){
                killTimer(timerID);
                timerActive = false;
            }
            else{
                timerID = startTimer(33);
                timerActive = true;
            }
            break;
        case Qt::Key_T:
            state.toggleEditMode();
            break;
        case Qt::Key_D:
            if(state.editMode) state.addIsoline();
            break;
        case Qt::Key_F:
            if(state.editMode) state.removeIsoLine();
            break;
        case Qt::Key_Y:
            state.sLine = !state.sLine;
            break;
        case Qt::Key_X:
            if(state.editMode)state.toggleEditModes();
            break;
        default:
            return;
    }
    // Redraw OpenGL.
    update();
}


void OpenGLDisplayWidget::updateMVPMatrix()
{
    // Calculate a simple model view transformation from rotation angles
    // and distance to camera.
    // NOTE: Read from bottom to top.

    QMatrix4x4 mvMatrix;
    mvMatrix.translate(0.0, 0.0, distanceToCamera);
    mvMatrix.rotate(rotationAngles.y(), QVector3D(1.0, 0.0, 0.0));
    mvMatrix.rotate(rotationAngles.x(), QVector3D(0.0, 1.0, 0.0));
    mvMatrix.rotate(-90,QVector3D(1,0,0));
    mvMatrix.translate(-1.0, -1.0, -1.0);
    mvMatrix.scale(2.0);
    mvpMatrix = projectionMatrix * mvMatrix;
}


void OpenGLDisplayWidget::initVisualizationPipeline()
{
    // Initialize the visualization pipeline:

    // Initialize data source(s).
    dataSource = new FlowDataSource(0);
    this->time = dataSource->getTime();
    this->component = X;
    this->slice = 15;
    // Initialize mapper modules.
    hSliceMapper = new HorizontalSliceToImageMapper(dataSource);
    hSliceMapper->setImageSource("../act.png");
    hContourMapper = new HorizontalSliceToContourLineMapper(dataSource);
    // Initialize rendering modules.
    bboxRenderer = new DataVolumeBoundingBoxRenderer();
    hSliceRenderer = new HorizontalSliceRenderer(hSliceMapper);
    hContourRenderer = new HorizontalContourLinesRenderer(hContourMapper);
    sLineMapper = new streamLineMapper(dataSource);
    sLineRenderer = new streamLineRenderer(sLineMapper);
    painter = new QPainter();
    // ....
}

bool OpenGLDisplayWidget::changeTime(int time) {
    if(this->time + time >= 0)
    {
        this->time += time;
        return false;
    }
    return true;
}

void OpenGLDisplayWidget::moveSliceForward() {
    if(slice + 1 > 31)
    {
        slice = 0;
        return;
    }
    slice += 1;
}

void OpenGLDisplayWidget::moveSliceBackward() {
    if(slice - 1 < 0)
    {
        slice = 31;
        return;
    }
    slice -= 1;
}
