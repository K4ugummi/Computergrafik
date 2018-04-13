// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#include "myglwidget.h"

// Normalized direction vectors.
const QVector3D MYQV_FRONT = QVector3D(1.0f, 0.0f, 0.0f);
const QVector3D MYQV_BACK = QVector3D(-1.0f, 0.0f, 0.0f);
const QVector3D MYQV_UP = QVector3D(0.0f, 1.0f, 0.0f);
const QVector3D MYQV_DOWN = QVector3D(0.0f, -1.0f, 0.0f);
const QVector3D MYQV_LEFT = QVector3D(0.0f, 0.0f, -1.0f);
const QVector3D MYQV_RIGHT = QVector3D(0.0f, 0.0f, 1.0f);

MyGLWidget::MyGLWidget(QWidget * parent) : QOpenGLWidget(parent) {
    m_CameraPos = QVector3D(0.0f, 0.0f, 0.0f);
}

MyGLWidget::~MyGLWidget() {

}

void MyGLWidget::setFOV(int value) {
    if (m_FOV != value) {
        m_FOV = value;
        //qDebug("MyGLWidget::setFOV: %i", value);
    }
}

void MyGLWidget::setAngle(int value) {
    if (m_Angle != value) {
        m_Angle = value;
        //qDebug("MyGLWidget::setAngle: %i", value);
    }
}

void MyGLWidget::setProjectionMode() {
    //qDebug("MyGLWidget::setProjectionMode()");
    // TODO: Wie kann ich bei einer Funktion für beide RadioButtons
    //       unterscheiden, welcher von beiden nach einem Click
    //       markiert wurde?
    //       Dieser Slot wurde in der Aufgabe vorgegeben, ich haette
    //       stattdessen zwei Slots 'setProjPerspektive()' und
    //       'setProjOrthogonal()' benutzt um eine Membervariable
    //       'bool m_isPerspective' auf true oder false zu setzen.
}

void MyGLWidget::setNear(double value) {
    if (m_NearClipping != value) {
        m_NearClipping = value;
        //qDebug("MyGLWidget::setNear: %f", value);
    }
}

void MyGLWidget::setFar(double value) {
    if (m_FarClipping != value) {
        m_FarClipping = value;
        //qDebug("MyGLWidget::setFar: %f", value);
    }
}

void MyGLWidget::setRotationA(int value) {
    if (m_RotationA != value) {
        m_RotationA = value;
        //qDebug("MyGLWidget::setRotationA: %i", value);
    }
}

void MyGLWidget::setRotationB(int value) {
    if (m_RotationB != value) {
        m_RotationB = value;
        //qDebug("MyGLWidget::setRotationB: %i", value);
    }
}

void MyGLWidget::setRotationC(int value) {
    if (m_RotationC != value) {
        m_RotationC = value;
        //qDebug("MyGLWidget::setRotationC: %i", value);
    }
}

void MyGLWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        //qDebug("MyGLWidget::keyPressEvent(Key_Escape)");
    }
    else if (event->key() == Qt::Key_W || event->key() == Qt::Key_Up) {
        //qDebug("MyGLWidget::keyPressEvent(Key_W|Key_Up)");
        m_CameraPos += MYQV_FRONT * 0.2f;
    }
    else if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) {
        //qDebug("MyGLWidget::keyPressEvent(Key_A|Key_Left)");
        m_CameraPos += MYQV_LEFT * 0.2f;
    }
    else if (event->key() == Qt::Key_S || event->key() == Qt::Key_Down) {
        //qDebug("MyGLWidget::keyPressEvent(Key_S|Key_Down)");
        m_CameraPos += MYQV_BACK * 0.2f;
    }
    else if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) {
        //qDebug("MyGLWidget::keyPressEvent(Key_D|Key_Right)");
        m_CameraPos += MYQV_RIGHT * 0.2f;
    }
    else {
        QOpenGLWidget::keyPressEvent(event);
    }
    qDebug("MyGLWidget::m_CameraPos(%.1f, %.1f, %.1f)",
           m_CameraPos.x(), m_CameraPos.y(), m_CameraPos.z());
}
