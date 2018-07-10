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

    m_FOV               = 35.0f;
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

    bool success = initializeOpenGLFunctions();
    Q_ASSERT(success);
    Q_UNUSED(success);

    initGLDebugger();

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_prog = new QOpenGLShaderProgram();
    m_prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/oproom.vert");
    m_prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/oproom.frag");
    m_prog->link();
    Q_ASSERT(m_prog->isLinked());

    MeshOPRoom * chair = new MeshOPRoom("chair");
    chair->setProgram(m_prog);
    m_meshes.push_back(chair);

    MeshOPRoom * floor = new MeshOPRoom("floor");
    floor->setProgram(m_prog);
    m_meshes.push_back(floor);

    MeshOPRoom * wall = new MeshOPRoom("wall");
    wall->setProgram(m_prog);
    m_meshes.push_back(wall);

    MeshOPRoom * lamp = new MeshOPRoom("lamp");
    lamp->setProgram(m_prog);
    m_meshes.push_back(lamp);

    MeshOPRoom * lamproof = new MeshOPRoom("lamproof");
    lamproof->setProgram(m_prog);
    m_meshes.push_back(lamproof);

    MeshOPRoom * roof = new MeshOPRoom("roof");
    roof->setProgram(m_prog);
    m_meshes.push_back(roof);

    MeshOPRoom * table = new MeshOPRoom("table");
    table->setProgram(m_prog);
    m_meshes.push_back(table);

    MeshOPRoom * homer = new MeshOPRoom("homer");
    homer->setProgram(m_prog);
    m_meshes.push_back(homer);

    MeshOPRoom * name = new MeshOPRoom("name");
    name->setProgram(m_prog);
    m_meshes.push_back(name);

    MeshOPRoom * notausgang = new MeshOPRoom("notausgang");
    notausgang->setProgram(m_prog);
    m_meshes.push_back(notausgang);

    MeshOPRoom * xray = new MeshOPRoom("xray");
    xray->setProgram(m_prog);
    m_meshes.push_back(xray);

    MeshOPRoom * protection = new MeshOPRoom("protection");
    protection->setProgram(m_prog);
    m_meshes.push_back(protection);

    MeshOPRoom * door = new MeshOPRoom("door");
    door->setProgram(m_prog);
    m_meshes.push_back(door);

    MeshOPRoom * curtain = new MeshOPRoom("curtain");
    curtain->setProgram(m_prog);
    m_meshes.push_back(curtain);

    m_skybox = new Skybox();
}

MyGLWidget::~MyGLWidget() {
    makeCurrent();

    delete m_prog;
    delete m_skybox;

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
    m_Camera.windowResize(width, height);
}

//
void MyGLWidget::paintGL() {
    float deltaTime = m_timer.elapsed();
    m_timer.start();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 proj;
    QMatrix4x4 projSkybox;
    proj.perspective(m_FOV, (float)m_width / (float)m_height, m_Near, m_Far);
    projSkybox.perspective(m_FOV, (float)m_width / (float)m_height, 0.1f, 10000.0f);

    QMatrix4x4 view = m_Camera.getViewMatrix();
    QVector3D position = m_Camera.getPosition();

    // Draw Skybox
    m_skybox->draw(projSkybox, view);

    // Draw meshes
    for (uint i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->draw(view, proj, position);
    }

    // Scedule this widget for repainting.
    update();
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
    m_RotationA = value;
}

void MyGLWidget::setRotationB(int value) {
    m_RotationB = value;
}

void MyGLWidget::setRotationC(int value) {
    m_RotationC = value;
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
    if (!m_debuglogger) {
        return;
    }

    if (message.severity() != QOpenGLDebugMessage::LowSeverity)
        qDebug() << message;
}
