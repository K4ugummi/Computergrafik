#ifndef CAMERA_H
#define CAMERA_H

#include <QPoint>
#include <QVector3D>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>

class Camera {

private:
    int m_width, m_height;

    QPoint m_lastMousePos;
    QPoint m_curMousePos;
    float m_arcballRadius;

    bool m_isModShift;
    bool m_isArcball;

    QVector3D m_position;
    QVector3D m_cameraFocusPos;
    QMatrix4x4 m_viewMat;

    void rotate();
    void translate();

    QVector3D calculateArcVec(int x, int y);

protected:
public:
    Camera();
    ~Camera();

    QMatrix4x4 getViewMatrix();
    QVector3D getPosition();

    void windowResize(int width, int height);

    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent * event);
};

#endif // CAMERA_H
