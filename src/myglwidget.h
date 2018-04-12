#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>

class MyGLWidget : public QOpenGLWidget {
    Q_OBJECT

private:
    int m_FOV;
    int m_Angle;
    bool m_IsProjection;
    double m_NearClipping;
    double m_FarClipping;
    int m_RotationA;
    int m_RotationB;
    int m_RotationC;

protected:

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
