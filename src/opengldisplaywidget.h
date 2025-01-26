#ifndef OPENGLDISPLAYWIDGET_H
#define OPENGLDISPLAYWIDGET_H

#include <QOpenGLWidget>
#include "datavolumeboundingboxrenderer.h"
#include "HorizontalSliceToImageMapper.h"
#include "HorizontalSliceRenderer.h"
#include "HorizontalSliceToContourLineMapper.h"
#include "HorizontalContourLinesRenderer.h"
#include "streamLineRenderer.h"
#include <QTimer>


class OpenGLDisplayWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit OpenGLDisplayWidget(QWidget *parent = nullptr);
    ~OpenGLDisplayWidget() override;

    QString openGLString();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;

    void keyPressEvent(QKeyEvent *e) override;
    void timerEvent(QTimerEvent *event);
private:
    int timerID;
    bool timerActive;
    void drawUI();
    stateControl state;
    // VIEW PROJECTION:
    // ================

    // Matrices and related variables that control vertex transformation
    // from world space to OpenGL view space.
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 mvpMatrix;
    QVector2D lastMousePosition;
    QVector2D rotationAngles;
    float distanceToCamera;
    // Recompute the mode-view-projection matrix from current rotation angles,
    // distance to camera, viewport geometry.
    void updateMVPMatrix();


    // VIISUALIZATION PIPELINE:
    // ========================
    DataVolumeBoundingBoxRenderer *bboxRenderer = NULL;
    HorizontalSliceToImageMapper *hSliceMapper = NULL;
    HorizontalSliceRenderer *hSliceRenderer = NULL;
    HorizontalSliceToContourLineMapper *hContourMapper = NULL;
    HorizontalContourLinesRenderer *hContourRenderer = NULL;
    streamLineRenderer *sLineRenderer = NULL;
    streamLineMapper *sLineMapper = NULL;
    FlowDataSource *dataSource = NULL;
    QPainter *painter = NULL;
    QTimer *timer = NULL;
    int slice;
    int component;
    int time;
    bool changeTime(int time);
    void moveSliceForward();
    void moveSliceBackward();
    // Initialize the pipeline (create instances of data source, mapping,
    // rendering etc. modules and connect them).
    void initVisualizationPipeline();
    QRgb colorMap[40] = {
            qRgb(92,200,242),
            qRgb(0,182,231),
            qRgb(0,165,220),
            qRgb(0,151,212),
            qRgb(0,139,205),
            qRgb(0,129,201),
            qRgb(0,117,190),
            qRgb(0,104,172),
            qRgb(0,92,156),
            qRgb(0,82,141),
            qRgb(0,73,127),
            qRgb(0,65,115),
            qRgb(0,57,104),
            qRgb(0,51,94),
            qRgb(0,45,83),
            qRgb(0,39,72),
            qRgb(0,33,60),
            qRgb(0,27,47),
            qRgb(0,22,34),
            qRgb(11,18,22),
            qRgb(23,15,15),
            qRgb(35,9,10),
            qRgb(45,4,5),
            qRgb(53,0,1),
            qRgb(61,0,0),
            qRgb(69,0,0),
            qRgb(78,0,0),
            qRgb(87,0,0),
            qRgb(98,0,0),
            qRgb(111,0,0),
            qRgb(125,0,0),
            qRgb(140,0,4),
            qRgb(158,0,15),
            qRgb(177,11,26),
            qRgb(193,37,45),
            qRgb(204,64,70),
            qRgb(216,89,92),
            qRgb(229,112,115),
            qRgb(242,137,139),
            qRgb(255,162,164)
    };
    QRgb colorMap2[100]
            {
                    qRgb(158,176,255),
                    qRgb(152,175,252),
                    qRgb(147,175,250),
                    qRgb(140,174,246),
                    qRgb(135,173,244),
                    qRgb(128,172,241),
                    qRgb(123,172,238),
                    qRgb(116,170,235),
                    qRgb(108,169,230),
                    qRgb(103,168,227),
                    qRgb(96,165,223	),
                    qRgb(91,164,219	),
                    qRgb(84,160,213),
                    qRgb(79,158,209),
                    qRgb(72,154,202),
                    qRgb(67,149,195),
                    qRgb(63,146,190),
                    qRgb(59,141,183),
                    qRgb(56,137,178),
                    qRgb(53,132,170),
                    qRgb(50,126,163),
                    qRgb(48,122,158),
                    qRgb(45,117,151),
                    qRgb(44,113,146),
                    qRgb(41,107,139),
                    qRgb(40,104,134),
                    qRgb(38,98,127),
                    qRgb(36,93,120),
                    qRgb(34,89,115),
                    qRgb(32,84,108),
                    qRgb(31,80,104),
                    qRgb(29,75,97),
                    qRgb(28,71,92),
                    qRgb(26,66,86),
                    qRgb(24,61,79),
                    qRgb(23,58,75),
                    qRgb(21,53,68),
                    qRgb(20,50,64),
                    qRgb(19,45,58),
                    qRgb(18,42,54),
                    qRgb(17,38,48),
                    qRgb(17,33,42),
                    qRgb(16,31,38),
                    qRgb(17,27,33),
                    qRgb(17,25,30),
                    qRgb(17,21,25),
                    qRgb(17,19,23),
                    qRgb(19,17,18),
                    qRgb(21,14,14),
                    qRgb(23,13,11),
                    qRgb(26,12,8),
                    qRgb(28,11,6),
                    qRgb(32,11,4),
                    qRgb(35,12,2),
                    qRgb(37,12,1),
                    qRgb(40,13,1),
                    qRgb(43,14,1),
                    qRgb(47,14,0),
                    qRgb(49,15,0),
                    qRgb(53,16,0),
                    qRgb(57,17,0),
                    qRgb(60,17,1),
                    qRgb(65,18,1),
                    qRgb(68,19,1),
                    qRgb(72,21,2),
                    qRgb(75,22,2),
                    qRgb(80,24,3),
                    qRgb(86,26,5),
                    qRgb(89,28,7),
                    qRgb(95,31,10),
                    qRgb(99,33,12),
                    qRgb(106,37,16),
                    qRgb(110,40,19),
                    qRgb(117,45,23),
                    qRgb(123,50,28),
                    qRgb(128,54,32),
                    qRgb(134,59,38),
                    qRgb(138,63,42),
                    qRgb(144,68,48),
                    qRgb(148,72,52),
                    qRgb(154,77,59),
                    qRgb(160,83,65),
                    qRgb(164,87,70),
                    qRgb(170,92,76),
                    qRgb(174,96,81),
                    qRgb(180,102,88),
                    qRgb(186,107,95),
                    qRgb(190,111,99),
                    qRgb(196,117,106),
                    qRgb(200,121,111),
                    qRgb(206,127,118),
                    qRgb(210,131,122),
                    qRgb(217,137,130),
                    qRgb(223,143,137),
                    qRgb(227,147,142),
                    qRgb(234,153,149),
                    qRgb(238,157,154),
                    qRgb(244,163,161),
                    qRgb(249,167,166),
                    qRgb(255,173,173)
            };
};

#endif //OPENGLDISPLAYWIDGET_H
