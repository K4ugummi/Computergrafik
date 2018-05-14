// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#include "skybox.h"

Skybox::Skybox() {
    bool success = initializeOpenGLFunctions();
    Q_ASSERT(success);
    Q_UNUSED(success);

    m_vertices = {
        -1,  1, -1,
        -1, -1, -1,
        1, -1, -1,
        1,  1, -1,
        -1, -1,  1,
        -1,  1,  1,
        1, -1,  1,
        1,  1,  1,
    };

    m_indices = {
        0, 1, 2, 2, 3, 0,
        4, 1, 0, 0, 5, 4,
        2, 6, 7, 7, 3, 2,
        4, 5, 7, 7, 6, 4,
        0, 3, 7, 7, 5, 0,
        1, 4, 2, 2, 4, 6,
    };

    QImage frontImg;
    frontImg.load(":/textures/skybox/front.jpg");
    Q_ASSERT(!frontImg.isNull());

    QImage backImg;
    backImg.load(":/textures/skybox/back.jpg");
    Q_ASSERT(!frontImg.isNull());

    QImage leftImg;
    leftImg.load(":/textures/skybox/left.jpg");
    Q_ASSERT(!leftImg.isNull());

    QImage rightImg;
    rightImg.load(":/textures/skybox/right.jpg");
    Q_ASSERT(!rightImg.isNull());

    QImage topImg;
    topImg.load(":/textures/skybox/top.jpg");
    Q_ASSERT(!topImg.isNull());

    QImage bottomImg;
    bottomImg.load(":/textures/skybox/bottom.jpg");
    Q_ASSERT(!bottomImg.isNull());

    // VAO BIND
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // IBO
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*m_indices.size(), &m_indices.front(), GL_STATIC_DRAW);

    // VBO BIND
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_vertices.size(), &m_vertices.front(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, nullptr);

    // TEX BIND
    glGenTextures(1, &m_cubeTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, rightImg.width(), rightImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, rightImg.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, leftImg.width(), leftImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, leftImg.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, topImg.width(), topImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, topImg.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, bottomImg.width(), bottomImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, bottomImg.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, backImg.width(), backImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, backImg.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, frontImg.width(), frontImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, frontImg.bits());

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // VAO UNBIND
    glBindVertexArray(0);

    m_prog = new QOpenGLShaderProgram();
    m_prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/skybox.vert");
    m_prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/skybox.frag");
    m_prog->link();
}

Skybox::~Skybox() {
    delete m_prog;

    glDeleteTextures(1, &m_cubeTex);
}

void Skybox::draw(const QMatrix4x4 &projection, QMatrix4x4 view) {
    glDepthMask(GL_FALSE);

    view.column(3) = QVector4D(0.0f, 0.0f, 0.0f, 0.0f);
    view.scale(10.0f);

    glBindVertexArray(m_vao);

    //glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex);

    m_prog->bind();
    m_prog->setUniformValue(0, projection);
    m_prog->setUniformValue(1, view);
    m_prog->setUniformValue(7, 0);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
}
