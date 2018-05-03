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

    m_CameraPos = QVector3D();
}

// Initialize OpenGL helper functions
void MyGLWidget::initializeGL() {
    qDebug("MyGLWidget::initializeGL()");
    initGLDebugger();

    bool success = initializeOpenGLFunctions();
    Q_ASSERT(success);
    Q_UNUSED(success);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_prog = new QOpenGLShaderProgram();
    m_prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/sample.vert");
    m_prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/sample.frag");
    m_prog->link();

    m_prog_texture = new QOpenGLShaderProgram();
    m_prog_texture->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/sample.vert");
    m_prog_texture->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/sample_texture.frag");
    m_prog_texture->link();

    Mesh * mesh = new Mesh();
    mesh->setProgram(m_prog);
    m_meshes.push_back(mesh);

    Q_ASSERT(m_prog->isLinked());
    Q_ASSERT(m_prog_texture->isLinked());
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

    qDebug().nospace() << "    OpenGL " << fmt.majorVersion() << "." << fmt.minorVersion();
    qDebug().noquote() << "    Profile:"
        << QMetaEnum::fromType<QSurfaceFormat::OpenGLContextProfile>().valueToKey(fmt.profile());
    qDebug().noquote() << "    Options:" << QMetaEnum::fromType<QSurfaceFormat::FormatOption>().valueToKeys(fmt.options());
    qDebug().noquote() << "    Renderable Type:"
        << QMetaEnum::fromType<QSurfaceFormat::RenderableType>().valueToKey(fmt.renderableType());
    qDebug().noquote() << "    Swap Behavior:"
        << QMetaEnum::fromType<QSurfaceFormat::SwapBehavior>().valueToKey(fmt.swapBehavior());
    qDebug() << "    Swap Interval:" << fmt.swapInterval();
}

void MyGLWidget::resizeGL(int width, int height) {
    m_width = width;
    m_height = height;
    qDebug("Width: %i - Height %i", m_width, m_height);
}

void MyGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    QMatrix4x4 mvp;
    mvp.perspective(m_Angle, m_width / m_height, m_Near, m_Far);
    m_prog->setUniformValue(0, mvp);

    for (uint i=0; i<m_meshes.size(); i++) {
        m_meshes[i]->draw();
    }
    update();
}

MyGLWidget::~MyGLWidget() {
    makeCurrent();

    delete m_prog;

    for(uint i = 0; i < m_meshes.size(); i++) {
        delete m_meshes[i];
    }

    delete m_debuglogger;
    doneCurrent();
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
    for (uint i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->rotate((float)value, QVector3D(1, 0, 0));
    }
}

void MyGLWidget::setRotationB(int value) {
    m_RotationB = value;
    for (uint i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->rotate((float)value, QVector3D(0, 1, 0));
    }
}

void MyGLWidget::setRotationC(int value) {
    m_RotationC = value;
    for (uint i = 0; i < m_meshes.size(); i++) {
        m_meshes[i]->rotate((float)value, QVector3D(0, 0, 1));
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
