// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include "meshoproom.h"
#include "skybox.h"
#include "camera.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>
#include <QOpenGLBuffer>
#include <QElapsedTimer>

#include <vector>

class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_3_Core {
    Q_OBJECT

private:
    // UI-Control values.
    int m_FOV;
    int m_Angle;
    bool m_IsProjPerspective;
    float m_Near, m_Far;
    float m_RotationA, m_RotationB, m_RotationC;
    bool m_AnimateGimbal, m_AnimateCamera;

    int m_width, m_height;

    QElapsedTimer m_timer;

    // Camera values.
    Camera m_Camera;

    // OpenGL
    QOpenGLDebugLogger * m_debuglogger;

    Skybox * m_skybox;
    std::vector<MeshOPRoom*> m_meshes;

    QOpenGLShaderProgram * m_prog;

    void initParam();
    void initGLDebugger();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent * event);

public:
    MyGLWidget(QWidget * parent);
    ~MyGLWidget();

public slots:
    // Slots for UI interaction.
    void setFOV(int value);
    void setAngle(int value);
    void setProjPerspective();
    void setProjOrthogonal();
    void setNear(double value);
    void setFar(double value);
    void setRotationA(int value);
    void setRotationB(int value);
    void setRotationC(int value);
    void setAnimateGimbal(bool value);
    void setAnimateCamera(bool value);

    // Slots for OGL signals.
    void onOGLMessage(QOpenGLDebugMessage message);

signals:
    void adjustNear(double value);
    void adjustFar(double value);
    void signalRotationA(int value);
    void signalRotationB(int value);
    void signalRotationC(int value);
};

#endif // MYGLWIDGET_H
