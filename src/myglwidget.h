#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>

class MyGLWidget : public QOpenGLWidget {
private:

protected:

public:
    MyGLWidget(QWidget * parent);
    ~MyGLWidget();

public slots:
    setFOV(int value);
    setAngle(int value);
    setProjectionMode();
    setNear(double value);
    setFar(double value);
    setRotationA(int value);
    setRotationB(int value);
    setRotationC(int value);
};

#endif // MYGLWIDGET_H
