// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QVector3D>
#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>

class MyGLWidget : public QOpenGLWidget {
    Q_OBJECT

private:
    // UI-Control values   -   Defaults.
    int m_FOV                = 45;
    int m_Angle              = 0;
    bool m_IsProjPerspective = true;
    float m_NearClipping     = 0.1f;
    float m_FarClipping      = 100.0f;
    int m_RotationA          = 0;
    int m_RotationB          = 0;
    int m_RotationC          = 0;

    // Camera values.
    QVector3D m_CameraPos;

    // OpenGL
    QOpenGLDebugLogger * m_debuglogger;

    void InitGL();

protected:
    void keyPressEvent(QKeyEvent *event);

public:
    MyGLWidget(QWidget * parent);
    ~MyGLWidget();

public slots:
    // Slots for UI interaction.
    void setFOV(int value);
    void setAngle(int value);
    void setProjectionMode();
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
