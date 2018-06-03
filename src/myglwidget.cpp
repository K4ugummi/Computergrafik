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
MyGLWidget::MyGLWidget(QWidget * parent) : QOpenGLWidget(parent) {
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

    m_Camera.windowResize(m_width, m_height);
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
    Q_ASSERT(m_prog->isLinked());

    m_prog_texture = new QOpenGLShaderProgram();
    m_prog_texture->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/sample.vert");
    m_prog_texture->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/sample_texture.frag");
    m_prog_texture->link();
    Q_ASSERT(m_prog_texture->isLinked());

    m_prog_phong = new QOpenGLShaderProgram();
    m_prog_phong->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/phong.vert");
    m_prog_phong->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/phong.frag");
    m_prog_phong->link();
    Q_ASSERT(m_prog_phong->isLinked());

    initLights();

    // Outer Gimbal
    Mesh * mesh = new Mesh(":/models/gimbal.obj");
    mesh->setProgram(m_prog_phong);
    mesh->setMaterial(Materials::PlasticRed);
    m_meshes.push_back(mesh);

    // Middle Gimbal
    Mesh * mesh2 = new Mesh(":/models/gimbal.obj");
    mesh2->setProgram(m_prog_phong);
    mesh2->setMaterial(Materials::PlasticGreen);
    mesh2->setScale(0.85f);
    m_meshes.push_back(mesh2);

    // Inner Gimbal
    Mesh * mesh3 = new Mesh(":/models/gimbal.obj");
    mesh3->setProgram(m_prog_phong);
    mesh3->setMaterial(Materials::PlasticYellow);
    mesh3->setScale(0.72f);
    m_meshes.push_back(mesh3);

    // Sphere
    m_ball = new Mesh(":/models/sphere.obj");
    m_ball->setProgram(m_prog_phong);
    m_ball->setMaterial(Materials::RubberGreen);
    m_ball->setScale(0.1f);

    m_skybox = new Skybox();

    Q_ASSERT(m_meshes.size() == 3);
}

MyGLWidget::~MyGLWidget() {
    makeCurrent();

    delete m_prog;
    delete m_prog_texture;
    delete m_prog_phong;

    glDeleteBuffers(1, &m_uboLights);
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

void MyGLWidget::initLights() {
    // Yellow light
    m_lightSource[0] = LightSource {
        { 2.0, 0.0, 2.0, }, // position
        -1.0,               // pad0
        { 1.0, 1.0, 0.0, }, // color
        -1.0,               // pad1
        0.8,                // ka
        0.8,                // kd
        1.0,                // ks
        -1.0,               // pad2
        1.0,                // constant
        0.22,               // linear
        0.2,                // quadratic
        -1.0,               // pad3
    };

    // Blue light
    m_lightSource[1] = LightSource {
        { 0.0, 2.0, 2.0 },  // position
        -1.0,               // pad0
        { 0.0, 0.0, 1.0, }, // color
        -1.0,               // pad1
        0.8,                // ka
        0.8,                // kd
        1.0,                // ks
        -1.0,               // pad2
        1.0,                // constant
        0.22,               // linear
        0.2,                // quadratic
        -1.0,               // pad3
    };

    // White light
    m_lightSource[2] = LightSource {
        { 0.0, 0.0, 0.0, }, // position
        -1.0,               // pad0
        { 1.0, 1.0, 0.0, }, // color
        -1.0,               // pad1
        0.1,                // ka
        0.1,                // kd
        0.1,                // ks
        -1.0,               // pad2
        1.0,                // constant
        0.7,                // linear
        1.8,                // quadratic
        -1.0,               // pad3
    };

    // Purple light
    m_lightSource[3] = LightSource {
        { -2.0, 0.0, -2.0, },// position
        -1.0,               // pad0
        { 0.5, 1.0, 0.5, }, // color
        -1.0,               // pad1
        0.8,                // ka
        0.8,                // kd
        1.0,                // ks
        -1.0,               // pad2
        1.0,                // constant
        0.22,               // linear
        0.2,                // quadratic
        -1.0,               // pad3
    };

    // Red light
    m_lightSource[4] = LightSource {
        { 0.25, -1.25, -0.25, }, // position
        -1.0,               // pad0
        { 1.0, 0.0, 0.0, },  // color
        -1.0,               // pad1
        0.8,                // ka
        0.8,                // kd
        1.0,                // ks
        -1.0,               // pad2
        1.0,                // constant
        0.22,               // linear
        0.2,                // quadratic
        -1.0,               // pad3
    };

    for (uint i = 0; i < NUM_LIGHTS; i++) {
        m_lightSource[i].constant = 1.0;
        m_lightSource[i].linear = 0.022;
        m_lightSource[i].quadratic = 0.0019;
    }

    glGenBuffers(1, &m_uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
    glBufferData(GL_UNIFORM_BUFFER, NUM_LIGHTS * sizeof(LightSource), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// Resize callback function for MyGLWidget.
// Is called autiomatically by QT.
void MyGLWidget::resizeGL(int width, int height) {
    m_width = width;
    m_height = height;
    m_Camera.windowResize(width, height);
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
    QVector3D position;
    if (m_AnimateCamera && m_meshes.size() >= 3) {
        view = m_meshes[2]->getModel().inverted();
        position = QVector3D(0, 0, 0);
    }
    else {
        view = m_Camera.getViewMatrix();
        position = m_Camera.getPosition();
    }

    if (m_AnimateGimbal) {
        animateGimbal(deltaTime);
    }

    animateBall(deltaTime);

    // Lights
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
    // Calculate light positions.
    // TODO: ...
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_lightSource), &m_lightSource);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Draw Skybox
    m_skybox->draw(projSkybox, view);

    // Draw meshes
    for (uint i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->draw(view, proj, position);
    }
    m_ball->draw(view, proj, position);

    // Scedule this widget for repainting.
    update();
}

void MyGLWidget::animateGimbal(float deltaTime) {
    m_RotationA += deltaTime * 0.05f;
    if (m_RotationA >= 360.0f)
        m_RotationA -= 360.0f;

    m_RotationB += deltaTime * 0.1f;
    if (m_RotationB >= 360.0f)
        m_RotationB -= 360.0f;

    m_RotationC += deltaTime * 0.15f;
    if (m_RotationC >= 360.0f)
        m_RotationC -= 360.0f;

    rotateGimbal();

    emit signalRotationA((int)m_RotationA);
    emit signalRotationB((int)m_RotationB);
    emit signalRotationC((int)m_RotationC);
}

QVector3D changeColorTime(float time) {
    QVector3D color;
    color.setX(qMax(qCos(time), 0.0));
    color.setY(qMax(qCos(time-2*M_PI/3), 0.0));
    color.setZ(qMax(qCos(time-4*M_PI/3), 0.0));
    return color;
}

void MyGLWidget::animateBall(float deltaTime) {
    static double ballRotationTimer;

    ballRotationTimer += deltaTime*0.001;
    if (ballRotationTimer >= 2 * M_PI) {
        ballRotationTimer = 0.0;
    }

    QVector3D position = QVector3D(
                0.85f*qSin(ballRotationTimer),
                0.85f*qCos(ballRotationTimer),
                -0.17f);

    //float angle = qAcos(position.y() / position.length());
    float angle = - ballRotationTimer * M_PI * 18; // Don't ask me why....

    QMatrix4x4 ballRotation;
    ballRotation.rotate(-ballRotationTimer*180*M_PI, QVector3D(0,1,0));
    QMatrix4x4 ballRotationOnGimbal;
    ballRotationOnGimbal.rotate(angle, QVector3D(0,0,1));

    // Gimbal rotations
    QMatrix4x4 worldRotation;
    worldRotation.rotate(m_RotationA, QVector3D(1, 0, 0));
    worldRotation.rotate(m_RotationB, QVector3D(0, 1, 0));
    m_ball->setRotation(worldRotation * ballRotationOnGimbal * ballRotation);
    m_ball->setPosition(worldRotation * position);

    m_ball->setColor(changeColorTime(ballRotationTimer));
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

void MyGLWidget::keyPressEvent(QKeyEvent * event) {
    m_Camera.keyPressEvent(event);
    QOpenGLWidget::keyPressEvent(event);
}

void MyGLWidget::keyReleaseEvent(QKeyEvent * event) {
    m_Camera.keyReleaseEvent(event);
    QOpenGLWidget::keyReleaseEvent(event);
}

void MyGLWidget::mouseMoveEvent(QMouseEvent * event) {
    m_Camera.mouseMoveEvent(event);
    QOpenGLWidget::mouseMoveEvent(event);
}

void MyGLWidget::mousePressEvent(QMouseEvent * event) {
    m_Camera.mousePressEvent(event);
    QOpenGLWidget::mousePressEvent(event);
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent * event) {
    m_Camera.mouseReleaseEvent(event);
    QOpenGLWidget::mouseReleaseEvent(event);
}

void MyGLWidget::wheelEvent(QWheelEvent * event) {
    m_Camera.wheelEvent(event);
    QOpenGLWidget::wheelEvent(event);
}

void MyGLWidget::onOGLMessage(QOpenGLDebugMessage message) {
    if (message.severity() != QOpenGLDebugMessage::LowSeverity)
        qDebug() << message;
}
