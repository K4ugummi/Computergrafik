// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#include "myglwidget.h"

#include <QMetaEnum>
#include <QSurfaceFormat>
#include <QImage>

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

    m_FOV               = 45;
    m_Angle             = 0;
    m_IsProjPerspective = true;
    m_NearClipping      = 0.1f;
    m_FarClipping       = 100.0f;
    m_RotationA         = 0;
    m_RotationB         = 0;
    m_RotationC         = 0;

    m_CameraPos = QVector3D();
}

// Initialize OpenGL helper functions
void MyGLWidget::initializeGL() {
    qDebug("MyGLWidget::initializeGL()");
    initGLDebugger();

    bool success = initializeOpenGLFunctions();
    Q_ASSERT(success);
    Q_UNUSED(success);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // TRIANGLE                   POSITION          COLOR               UV-COORDS
    m_vertices.push_back(Vertex { -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,   0.25f, 0.25f });
    m_vertices.push_back(Vertex { 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,   0.75f, 0.25f });
    m_vertices.push_back(Vertex { 0.0f, 0.5f,       0.0f, 0.0f, 1.0f,   0.5f, 0.75f });

    QImage img;
    img.load(":/textures/sample_texture.jpg");
    Q_ASSERT(!img.isNull());

#define OFS(s, a) reinterpret_cast<void* const>(offsetof(s, a))

    // VAO BIND
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // VBO BIND
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), &m_vertices.front(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex, color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex, uv));

    // VAO UNBIND
    glBindVertexArray(0);

    // TEX BIND
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE, m_tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.width(), img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_prog = new QOpenGLShaderProgram();
    m_prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/sample.vert");
    m_prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/sample.frag");
    m_prog->link();

    Q_ASSERT(m_prog->isLinked());

#undef OFS

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

}

void MyGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(m_vao);

    glBindTexture(GL_TEXTURE_2D, m_tex);

    m_prog->bind();
    m_prog->setUniformValue(0, (float)m_RotationA/360.0f);

    //später glDrawElements()
    glDrawArrays(GL_TRIANGLES, 0, 3);

    update();
}

MyGLWidget::~MyGLWidget() {
    makeCurrent();

    delete m_prog;
    glDeleteTextures(1, &m_tex);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);

    delete m_debuglogger;
    doneCurrent();
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

void MyGLWidget::setProjPerspective() {
    if (!m_IsProjPerspective) {
        m_IsProjPerspective = true;
    }
}

void MyGLWidget::setProjOrthogonal() {
    if (m_IsProjPerspective) {
        m_IsProjPerspective = false;
    }
}

void MyGLWidget::setNear(double value) {
    if (m_NearClipping != value) {
        m_NearClipping = value;
        if (m_NearClipping > m_FarClipping - 2.0f) {
            emit adjustFar(m_NearClipping + 2.0f);
        }
        //qDebug("MyGLWidget::setNear: %f", value);
    }
}

void MyGLWidget::setFar(double value) {
    if (m_FarClipping != value) {
        m_FarClipping = value;
        if (m_FarClipping < m_NearClipping + 2.0f) {
            emit adjustNear(m_FarClipping - 2.0f);
        }
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

void MyGLWidget::onOGLMessage(QOpenGLDebugMessage message) {
    qDebug() << message;
}
