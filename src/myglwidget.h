// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include "mesh.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>
#include <QOpenGLBuffer>

#include <vector>

class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core {
    Q_OBJECT

private:
    // UI-Control values.
    int m_FOV;
    int m_Angle;
    bool m_IsProjPerspective;
    float m_Near, m_Far;
    float m_RotationA, m_RotationB, m_RotationC;

    int m_width, m_height;

    // Camera values.
    QVector3D m_CameraPos;

    // OpenGL
    QOpenGLDebugLogger * m_debuglogger;

    std::vector<Mesh*> m_meshes;

    QOpenGLShaderProgram * m_prog;
    QOpenGLShaderProgram * m_prog_texture;

    void initParam();
    void initGLDebugger();

    void rotateFromID(uint id, GLfloat angle, QVector3D axis);

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void keyPressEvent(QKeyEvent *event);

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

    // Slots for OGL signals.
    void onOGLMessage(QOpenGLDebugMessage message);

signals:
    void adjustNear(double value);
    void adjustFar(double value);
};

#endif // MYGLWIDGET_H
