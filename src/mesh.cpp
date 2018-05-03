// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#include "mesh.h"
#include "modelloaderh.h"

#define OFS(s, a) reinterpret_cast<void* const>(offsetof(s, a))

Mesh::Mesh() {
    bool success = initializeOpenGLFunctions();
    Q_ASSERT(success);
    Q_UNUSED(success);

    ModelLoader modelloader;
    if (!modelloader.loadObjectFromFile(":/models/gimbal.obj")) {
        qWarning("Could not load obj");
    }
    std::vector<GLfloat> vbo;
    vbo.reserve(modelloader.lengthOfVBO());
    modelloader.genVBO(vbo.data());
    Q_ASSERT(vbo.capacity() % 8 == 0);

    for (uint i = 0; i < modelloader.lengthOfVBO(); i += 8 ) {
        m_vertices.push_back( Vertex { vbo[i], vbo[i+1], vbo[i+2], vbo[i+3], vbo[i+4], vbo[i+5], vbo[i+6], vbo[i+7], } );
    }

    std::vector<GLuint> ibo;
    ibo.reserve(modelloader.lengthOfIndexArray());
    modelloader.genIndexArray(ibo.data());
    for (uint i = 0; i < modelloader.lengthOfIndexArray(); i++) {
        m_indices.push_back(ibo[i]);
    }

    /*
    // TRIANGLE                   POSITION          COLOR               UV-COORDS
    m_vertices.push_back(Vertex { -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,   0.25f, 0.25f, });
    m_vertices.push_back(Vertex { 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,   0.75f, 0.25f, });
    m_vertices.push_back(Vertex { 0.0f, 0.5f,       0.0f, 0.0f, 1.0f,   0.5f, 0.75f, });
    m_vertices.push_back(Vertex { 1.0f, 0.5f,       1.0f, 1.0f, 1.0f,   1.0f, 0.75f, });

    GLuint data[] = { 0, 1, 2, 2, 1, 3, };
    */

    QImage img;
    img.load(":/textures/sample_texture.jpg");
    Q_ASSERT(!img.isNull());

    // VAO BIND
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // IBO
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* m_indices.size(), &m_indices.front(), GL_STATIC_DRAW);

    // VBO BIND
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), &m_vertices.front(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex, uv));

    // TEX BIND
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.width(), img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // VAO UNBIND
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteTextures(1, &m_tex);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
}

void Mesh::setProgram(QOpenGLShaderProgram * prog) {
    m_prog = prog;
}

void Mesh::rotate(GLfloat angle, QVector3D axis) {
    m_rotation = QMatrix4x4();
    m_rotation.rotate(angle, axis);
}

void Mesh::draw() {
    Q_ASSERT(m_prog->isLinked());

    glBindVertexArray(m_vao);

    m_prog->bind();

    //void * const offset = reinterpret_cast<void * const>(sizeof(GLuint)*3);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
}
