// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#include "myglwidget.h"

#include <QMetaEnum>
#include <QSurfaceFormat>
#include <QImage>

#define OFS(s, a) reinterpret_cast<void* const>(offsetof(s, a))

// Normalized direction vectors.
const QVector3D MYQV_FRONT = QVector3D(1.0f, 0.0f, 0.0f);
const QVector3D MYQV_BACK = QVector3D(-1.0f, 0.0f, 0.0f);
const QVector3D MYQV_UP = QVector3D(0.0f, 1.0f, 0.0f);
const QVector3D MYQV_DOWN = QVector3D(0.0f, -1.0f, 0.0f);
const QVector3D MYQV_LEFT = QVector3D(0.0f, 0.0f, -1.0f);
const QVector3D MYQV_RIGHT = QVector3D(0.0f, 0.0f, 1.0f);

// Contructor of a customizable OpenGL widget.
MyGLWidget::MyGLWidget(QWidget * parent)
    : QOpenGLWidget(parent)
{
    initParam();
}

// Initialize default parameter.
void MyGLWidget::initParam() {
    qDebug("MyGLWidget::InitParam()");

    m_FOV               = 45.0f;
    m_Angle             = 0.0f;
    m_IsProjPerspective = true;
    m_Near              = 0.1f;
    m_Far               = 100.0f;
    m_RotationA         = 0.0f;
    m_RotationB         = 0.0f;
    m_RotationC         = 0.0f;
    m_AnimateCamera     = false;
    m_AnimateGimbal     = false;

    m_CameraPos = QVector3D(0.0f, 0.0f, -5.0f);
}

// Initialize OpenGL helper functions
void MyGLWidget::initializeGL() {
    m_timer.start();
    qDebug("MyGLWidget::initializeGL()");
    initGLDebugger();

    bool success = initializeOpenGLFunctions();
    Q_ASSERT(success);
    Q_UNUSED(success);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_prog = new QOpenGLShaderProgram();
    m_prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/sample.vert");
    m_prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/sample.frag");
    m_prog->link();

    m_prog_texture = new QOpenGLShaderProgram();
    m_prog_texture->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/sample.vert");
    m_prog_texture->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/sample_texture.frag");
    m_prog_texture->link();

    Q_ASSERT(m_prog->isLinked());
    Q_ASSERT(m_prog_texture->isLinked());

    // Outer Gimbal
    Mesh * mesh = new Mesh(":/models/gimbal.obj");
    mesh->rotateRawZ(1.570796f);
    mesh->setProgram(m_prog_texture);
    mesh->setColor(QVector3D(1.0f, 0.3f, 0.3f));
    m_meshes.push_back(mesh);

    // Middle Gimbal
    Mesh * mesh2 = new Mesh(":/models/gimbal.obj");
    mesh2->setProgram(m_prog_texture);
    mesh2->setColor(QVector3D(0.3f, 1.0f, 0.3f));
    mesh2->scale(0.85f);
    m_meshes.push_back(mesh2);

    // Inner Gimbal
    Mesh * mesh3 = new Mesh(":/models/gimbal.obj");
    mesh3->rotateRawZ(1.570796f);
    mesh3->setProgram(m_prog_texture);
    mesh3->setColor(QVector3D(0.3f, 0.3f, 1.0f));
    mesh3->scale(0.72f);
    m_meshes.push_back(mesh3);

    // Sphere
    m_ball = new Mesh(":/models/sphere.obj");
    m_ball->setProgram(m_prog_texture);
    m_ball->setColor(QVector3D(1.0f, 1.0f, 1.0f));
    m_ball->scale(0.1f);
    m_ball->translate(QVector3D(0.0f, 5.0f, 0.0f));

    m_skybox = new Skybox();
}

MyGLWidget::~MyGLWidget() {
    makeCurrent();

    delete m_prog;
    delete m_prog_texture;

    delete m_skybox;
    delete m_ball;
    for(uint i = 0; i < m_meshes.size(); i++) {
        delete m_meshes[i];
    }

    delete m_debuglogger;
    doneCurrent();
}

void MyGLWidget::initGLDebugger() {
    m_debuglogger = new QOpenGLDebugLogger(this);

    connect(m_debuglogger, &QOpenGLDebugLogger::messageLogged,
            this, &MyGLWidget::onOGLMessage);

    if (m_debuglogger->initialize()) {
        m_debuglogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
        m_debuglogger->enableMessages();
    }

    QSurfaceFormat fmt = this->format();

    qDebug().nospace() << "  OpenGL " << fmt.majorVersion() << "." << fmt.minorVersion();
    qDebug().noquote() << "  Profile:"
        << QMetaEnum::fromType<QSurfaceFormat::OpenGLContextProfile>().valueToKey(fmt.profile());
    qDebug().noquote() << "  Options:" << QMetaEnum::fromType<QSurfaceFormat::FormatOption>().valueToKeys(fmt.options());
    qDebug().noquote() << "  Renderable Type:"
        << QMetaEnum::fromType<QSurfaceFormat::RenderableType>().valueToKey(fmt.renderableType());
    qDebug().noquote() << "  Swap Behavior:"
        << QMetaEnum::fromType<QSurfaceFormat::SwapBehavior>().valueToKey(fmt.swapBehavior());
    qDebug() << "  Swap Interval:" << fmt.swapInterval();
}

// Resize callback function for MyGLWidget.
// Is called autiomatically by QT.
void MyGLWidget::resizeGL(int width, int height) {
    m_width = width;
    m_height = height;
}

//
void MyGLWidget::paintGL() {
    static float deltaTime = m_timer.elapsed();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 proj;
    proj.perspective(m_FOV, (float)m_width / (float)m_height, m_Near, m_Far);
    QMatrix4x4 view;
    if (m_AnimateCamera && m_meshes.size() >= 3) {
        view = m_meshes[2]->getModel();
    }
    else {
        view.lookAt(m_CameraPos, QVector3D(0,0,0), MYQV_UP);
    }
    QMatrix4x4 vp = proj * view;

    if (m_AnimateGimbal) {
        animateGimbal(deltaTime);
    }

    m_skybox->draw(proj, view);
    for (uint i=0; i<m_meshes.size(); i++) {
        m_meshes[i]->draw(proj, view);
    }
    m_ball->draw(proj, view);

    // Scedule this widget for repainting.
    update();
}

void MyGLWidget::animateGimbal(float time) {
    animateA(time*0.001);
    animateB(time*0.0015);
    animateC(time*0.002);
    emit signalRotationA((int)m_RotationA);
    emit signalRotationB((int)m_RotationB);
    emit signalRotationC((int)m_RotationC);
}

void MyGLWidget::animateA(float time) {
    m_RotationA += time;
    if (m_RotationA >= 360.0f)
        m_RotationA -= 360.0f;

    rotateFromID(2, -m_RotationC, QVector3D(1, 0, 0));
    rotateFromID(1, -m_RotationB, QVector3D(0, 1, 0));

    rotateFromID(0, time, QVector3D(1, 0, 0));

    rotateFromID(1, m_RotationB, QVector3D(0, 1, 0));
    rotateFromID(2, m_RotationC, QVector3D(1, 0, 0));
}

void MyGLWidget::animateB(float time) {
    m_RotationB += time;
    if (m_RotationB >= 360.0f)
        m_RotationB -= 360.0f;

    rotateFromID(2, -m_RotationC, QVector3D(1, 0, 0));

    rotateFromID(1, time, QVector3D(0, 1, 0));

    rotateFromID(2, m_RotationC, QVector3D(1, 0, 0));
}

void MyGLWidget::animateC(float time) {
    m_RotationC += time;
    if (m_RotationC >= 360.0f)
        m_RotationC -= 360.0f;
    rotateFromID(2, time, QVector3D(1, 0, 0));
}

void MyGLWidget::setFOV(int value) {
    m_FOV = (float)value;
}

void MyGLWidget::setAngle(int value) {
    m_Angle = (float)value;
}

void MyGLWidget::setProjPerspective() {
    m_IsProjPerspective = true;
}

void MyGLWidget::setProjOrthogonal() {
    m_IsProjPerspective = false;
}

void MyGLWidget::setNear(double value) {
    m_Near = (float)value;
    if (m_Near > m_Far - 2.0f) {
        emit adjustFar(m_Near + 2.0f);
    }
}

void MyGLWidget::setFar(double value) {
    m_Far = value;
    if (m_Far < m_Near + 2.0f) {
        emit adjustNear(m_Far - 2.0f);
    }
}

void MyGLWidget::setRotationA(int value) {
    float dif = (float)value - m_RotationA;
    m_RotationA = value;

    rotateFromID(2, -m_RotationC, QVector3D(1, 0, 0));
    rotateFromID(1, -m_RotationB, QVector3D(0, 1, 0));

    rotateFromID(0, dif, QVector3D(1, 0, 0));

    rotateFromID(1, m_RotationB, QVector3D(0, 1, 0));
    rotateFromID(2, m_RotationC, QVector3D(1, 0, 0));
}

void MyGLWidget::setRotationB(int value) {
    float dif = (float)value - m_RotationB;
    m_RotationB = value;

    rotateFromID(2, -m_RotationC, QVector3D(1, 0, 0));

    rotateFromID(1, dif, QVector3D(0, 1, 0));

    rotateFromID(2, m_RotationC, QVector3D(1, 0, 0));
}

void MyGLWidget::setRotationC(int value) {
    float dif = (float)value - m_RotationC;
    m_RotationC = value;
    rotateFromID(2, dif, QVector3D(1, 0, 0));
}

void MyGLWidget::setAnimateCamera(bool value) {
    m_AnimateCamera = value;
}

void MyGLWidget::setAnimateGimbal(bool value) {
    m_AnimateGimbal = value;
}

void MyGLWidget::rotateFromID(uint id, GLfloat angle, QVector3D axis) {
    for (uint i = id; i < m_meshes.size(); i++) {
        m_meshes[i]->rotate(angle, axis);
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

void MyGLWidget::onOGLMessage(QOpenGLDebugMessage message) {
    qDebug() << message;
}
