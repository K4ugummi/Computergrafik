#ifndef CAMERA_H
#define CAMERA_H

#include <QPoint>
#include <QVector3D>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM = 45.0f;

// First person and arcball rotation camera.
class Camera {

private:
    // Window width and height.
    int m_width, m_height;

    // Key and mouse button as modifier.
    bool m_isModShift;
    bool m_isModAlt;
    bool m_isArcball;
    bool m_isPOV;

    // Arcball member.
    QPoint m_lastMousePos;
    QPoint m_curMousePos;
    float m_arcballRadius;
    QVector3D m_cameraFocusPos;

    // First person member.
    bool m_isW, m_isA, m_isS, m_isD; // WASD movement

    QVector3D m_position;
    QVector3D m_front;
    QVector3D m_up;
    QVector3D m_right;
    QVector3D m_worldUp;

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
