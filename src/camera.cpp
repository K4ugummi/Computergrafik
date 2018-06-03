#include "camera.h"

#include <qmath.h>

Camera::Camera() {
    m_isModShift = m_isArcball = false;
    m_position = QVector3D(0, 0, -4);
    m_viewMat.lookAt(m_position, QVector3D(0, 0, 0), QVector3D(0, 1, 0));
}

Camera::~Camera() {

}

void Camera::rotate() {
    QVector3D v = calculateArcVec(m_lastMousePos.x(), m_lastMousePos.y());
    QVector3D u = calculateArcVec(m_curMousePos.x(), m_curMousePos.y());

    float angle = std::acos(std::min(1.0f, QVector3D::dotProduct(u,v)));

    QVector3D axisCamera = QVector3D::crossProduct(v,u);
    QMatrix4x4 viewInv = m_viewMat.inverted();
    QVector4D axisObjH = viewInv * QVector4D(axisCamera, 1.0);

    QVector3D axisObj = QVector3D(axisObjH.x(), axisObjH.y(), axisObjH.z());

    m_viewMat.rotate(4 * qRadiansToDegrees(angle), axisObj);
}

void Camera::translate() {

}

QVector3D Camera::calculateArcVec(int x, int y) {
    QVector3D arcVec = QVector3D(
                x / m_width * 2 - 1.0,
                y / m_height * 2 - 1.0,
                0.0);
    arcVec.setY(-arcVec.y());

    // Calculate z-coordinate
    float xySquare = arcVec.x() * arcVec.x() + arcVec.y() * arcVec.y();
    if (xySquare <= 1.0) {
        arcVec.setZ(sqrt(1.0 - xySquare));
    }
    else {
        arcVec.normalize();
    }

    return arcVec;
}

QMatrix4x4 Camera::getViewMatrix() {
    return m_viewMat;
}

QVector3D Camera::getPosition() {
    return m_position;
}

void Camera::windowResize(int width, int height) {
    m_width = width;
    m_height = height;
}


void Camera::keyPressEvent(QKeyEvent * event) {
    if (event->key() == Qt::Key_Shift) {
        m_isModShift = true;
    }
}

void Camera::keyReleaseEvent(QKeyEvent * event) {
    if (event->key() == Qt::Key_Shift) {
        m_isModShift = false;
    }
}

void Camera::mouseMoveEvent(QMouseEvent * event) {
    if (m_isArcball) {
        m_curMousePos = event->pos();
        if (m_isModShift) {
            translate();
        }
        else {
            rotate();
        }
    }
    m_lastMousePos = m_curMousePos;
}

void Camera::mousePressEvent(QMouseEvent * event) {
    if (event->button() == Qt::LeftButton) {
        m_lastMousePos = m_curMousePos = event->pos();
        m_isArcball = true;
    }
}

void Camera::mouseReleaseEvent(QMouseEvent * event) {
    if (event->button() == Qt::LeftButton) {
        m_isArcball = false;
    }
}

void Camera::wheelEvent(QWheelEvent * event) {
    m_arcballRadius += event->delta();
}
