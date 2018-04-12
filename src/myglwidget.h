#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QOpenGLWidget>

class MyGLWidget : public QOpenGLWidget {
private:
protected:
public:
    MyGLWidget(QWidget * parent);
    ~MyGLWidget();
};

#endif // MYGLWIDGET_H
