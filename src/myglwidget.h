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
#include <QVector3D>
#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>
#include <QOpenGLBuffer>

#include <vector>

struct Vertex {
    GLfloat position[2];
    GLfloat color[3];
};

class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core {
    Q_OBJECT

private:
    // UI-Control values.
    int m_FOV;
    int m_Angle;
    bool m_IsProjPerspective;
    float m_NearClipping;
    float m_FarClipping;
    int m_RotationA;
    int m_RotationB;
    int m_RotationC;

    // Camera values.
    QVector3D m_CameraPos;

    // OpenGL
    QOpenGLDebugLogger * m_debuglogger;

    std::vector<Vertex> m_vertices;
    GLuint m_vbo;
    GLuint m_vao;
    QOpenGLShaderProgram * m_prog;

    void initParam();
    void initGLDebugger();

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
