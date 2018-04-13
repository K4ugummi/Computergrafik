#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QVector3D>

class MyGLWidget : public QOpenGLWidget {
    Q_OBJECT

private:
    // UI-Control values.
    int m_FOV;
    int m_Angle;
    bool m_IsProjection;
    double m_NearClipping;
    double m_FarClipping;
    int m_RotationA;
    int m_RotationB;
    int m_RotationC;

    // Camera values.
    QVector3D m_CameraPos;

protected:
    void keyPressEvent(QKeyEvent *event);

public:
    MyGLWidget(QWidget * parent);
    ~MyGLWidget();

public slots:
    void setFOV(int value);
    void setAngle(int value);
    void setProjectionMode();
    void setNear(double value);
    void setFar(double value);
    void setRotationA(int value);
    void setRotationB(int value);
    void setRotationC(int value);
};

#endif // MYGLWIDGET_H
