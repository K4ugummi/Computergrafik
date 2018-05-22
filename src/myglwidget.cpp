// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#include "myglwidget.h"

#include <QMetaEnum>
#include <QSurfaceFormat>
#include <QImage>
#include <QtMath>

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
    mesh2->setScale(0.85f);
    m_meshes.push_back(mesh2);

    // Inner Gimbal
    Mesh * mesh3 = new Mesh(":/models/gimbal.obj");
    mesh3->rotateRawZ(1.570796f);
    mesh3->setProgram(m_prog_texture);
    mesh3->setColor(QVector3D(0.3f, 0.3f, 1.0f));
    mesh3->setScale(0.72f);
    m_meshes.push_back(mesh3);

    // Sphere
    m_ball = new Mesh(":/models/sphere.obj");
    m_ball->setProgram(m_prog_texture);
    m_ball->setScale(0.1f);

    m_skybox = new Skybox();

    Q_ASSERT(m_meshes.size() == 3);
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
    float deltaTime = m_timer.elapsed();
    m_timer.start();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 proj;
    QMatrix4x4 projSkybox;
    if (m_IsProjPerspective) {
        proj.perspective(m_FOV, (float)m_width / (float)m_height, m_Near, m_Far);
        projSkybox.perspective(m_FOV, (float)m_width / (float)m_height, 0.1f, 10000.0f);
    }
    else {
        proj.ortho(-m_width/200, m_width/200, -m_height/200, m_height/200, m_Near, m_Far);
        projSkybox.ortho(-m_width/20, m_width/20, -m_height/20, m_height/20, 0.1f, 10000.0f);
    }

    QMatrix4x4 view;
    if (m_AnimateCamera && m_meshes.size() >= 3) {
        view = m_meshes[2]->getModel().inverted();
    }
    else {
        view.lookAt(m_CameraPos, QVector3D(0,0,0), MYQV_UP);
    }

    if (m_AnimateGimbal) {
        animateGimbal(deltaTime);
    }

    animateBall(deltaTime);

    m_skybox->draw(projSkybox, view);
    for (uint i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->draw(proj, view);
    }
    m_ball->draw(proj, view);

    // Scedule this widget for repainting.
    update();
}

void MyGLWidget::animateGimbal(float deltaTime) {
    m_RotationA += deltaTime * 0.1;
    if (m_RotationA >= 360.0f)
        m_RotationA -= 360.0f;

    m_RotationB += deltaTime * 0.07;
    if (m_RotationB >= 360.0f)
        m_RotationB -= 360.0f;

    m_RotationC += deltaTime * 0.05;
    if (m_RotationC >= 360.0f)
        m_RotationC -= 360.0f;

    rotateGimbal();

    emit signalRotationA((int)m_RotationA);
    emit signalRotationB((int)m_RotationB);
    emit signalRotationC((int)m_RotationC);
}

QVector3D changeColorTime(float time) {
    float colorValue = time;
}

void MyGLWidget::animateBall(float deltaTime) {
    static double ballRotationTimer;

    ballRotationTimer += deltaTime*0.001;
    if (ballRotationTimer > 2 * M_PI) {
        ballRotationTimer = 0.0;
    }
    qDebug() << ballRotationTimer;

    QVector3D position = QVector3D(
                0.85f*qSin(ballRotationTimer),
                0.85f*qCos(ballRotationTimer),
                -0.17f);

    //float angle = qAcos(position.y() / position.length());
    float angle = - ballRotationTimer * M_PI * 18;

    QMatrix4x4 ballRotation;
    ballRotation.rotate(-ballRotationTimer*1000, QVector3D(0,1,0));
    QMatrix4x4 ballRotationOnGimbal;
    ballRotationOnGimbal.rotate(angle, QVector3D(0,0,1));

    // Gimbal rotations
    QMatrix4x4 worldRotation;
    worldRotation.rotate(m_RotationA, QVector3D(1, 0, 0));
    worldRotation.rotate(m_RotationB, QVector3D(0, 1, 0));
    m_ball->setRotation(worldRotation * ballRotationOnGimbal * ballRotation);
    m_ball->setPosition(worldRotation * position);
}

void MyGLWidget::setFOV(int value) {
    m_FOV = (float)value;
}

void MyGLWidget::setAngle(int value) {
    m_Angle = (float)value;
}

void MyGLWidget::setProjPerspective() {
    m_IsProjPerspective = true;
    qDebug() << m_IsProjPerspective;
}

void MyGLWidget::setProjOrthogonal() {
    m_IsProjPerspective = false;
    qDebug() << m_IsProjPerspective;
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

void MyGLWidget::rotateGimbal() {
    QMatrix4x4 rotMA;
    rotMA.rotate(m_RotationA, QVector3D(1, 0, 0));
    QMatrix4x4 rotMB = rotMA;
    rotMB.rotate(m_RotationB, QVector3D(0, 1, 0));
    QMatrix4x4 rotMC = rotMB;
    rotMC.rotate(m_RotationC, QVector3D(1, 0, 0));

    Q_ASSERT(m_meshes.size() >= 3);
    m_meshes[0]->setRotation(rotMA);
    m_meshes[1]->setRotation(rotMB);
    m_meshes[2]->setRotation(rotMC);
}

void MyGLWidget::setRotationA(int value) {
    m_RotationA = value;
    rotateGimbal();
}

void MyGLWidget::setRotationB(int value) {
    m_RotationB = value;
    rotateGimbal();
}

void MyGLWidget::setRotationC(int value) {
    m_RotationC = value;
    rotateGimbal();
}

void MyGLWidget::setAnimateCamera(bool value) {
    m_AnimateCamera = value;
}

void MyGLWidget::setAnimateGimbal(bool value) {
    m_AnimateGimbal = value;
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
