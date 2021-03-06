// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#include "mesh.h"
#include "modelloaderh.h"

#define OFS(s, a) reinterpret_cast<void* const>(offsetof(s, a))

Mesh::Mesh(QString filepath) {
    bool success = initializeOpenGLFunctions();
    Q_ASSERT(success);
    Q_UNUSED(success);

    m_model = QMatrix4x4();
    m_scale = 1.0f;
    m_position = QVector3D(0.0f, 0.0f, 0.0f);
    m_rotation = QVector3D(0.0f, 0.0f, 0.0f);
    m_color = QVector3D(1.0f, 1.0f, 1.0f);

    ModelLoader modelloader;
    if (!modelloader.loadObjectFromFile(filepath)) {
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

    QImage img;
    img.load(":/textures/gimbal_wood.jpg");
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

void Mesh::setColor(QVector3D color) {
    m_color = color;
}

void Mesh::setMaterial(Material mat) {
    m_material = mat;
}

void Mesh::setModel(QMatrix4x4 model) {
    m_model = model;
}

void Mesh::setRotation(QMatrix4x4 rotMat) {
    m_model = rotMat;
    m_model.translate(m_position);
    m_model.scale(m_scale);
}

QVector3D Mesh::getColor() {
    return m_color;
}

QMatrix4x4 Mesh::getModel() {
    return m_model;
}

QVector3D Mesh::getPosition() {
    return m_model.column(3).toVector3D();
}

void Mesh::scale(GLfloat scale) {
    m_model.scale(scale);
}

void Mesh::rotate(GLfloat angle, QVector3D axis) {
    m_model.rotate(angle, axis);
}

void Mesh::rotate(QQuaternion quat) {
    m_model.rotate(quat);
}

void Mesh::setScale(GLfloat scale) {
    m_scale = scale;
    QMatrix4x4 model;
    model.rotate(m_rotation.x(), QVector3D(1,0,0));
    model.rotate(m_rotation.y(), QVector3D(0,1,0));
    model.rotate(m_rotation.z(), QVector3D(0,0,1));
    model.translate(m_position);
    model.scale(scale);
    m_model = model;
}

void Mesh::setPosition(QVector3D position) {
    m_model.setColumn(3, QVector4D(position, 1.0f));
}

// I tried rotating the uv to align these small black arrows with
// the rotation axis of the next gimbal element.
void Mesh::rotateRawZ(GLfloat angle) {
    for (uint i = 0; i < m_vertices.size(); i++) {
        float x = m_vertices[i].position[0];
        float y = m_vertices[i].position[1];

        m_vertices[i].position[0] = x * cos(angle) - y * sin(angle);
        m_vertices[i].position[1] = x * sin(angle) + y * cos(angle);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), &m_vertices.front(), GL_STATIC_DRAW);
}

void Mesh::translate(QVector3D translate) {
    m_model.translate(translate);
}

void Mesh::draw(const QMatrix4x4 &view, const QMatrix4x4 &proj, const QVector3D &viewPos) {
    Q_ASSERT(m_prog->isLinked());

    glBindVertexArray(m_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex);

    m_prog->bind();
    m_prog->setUniformValue(0, m_model);
    m_prog->setUniformValue(1, view);
    m_prog->setUniformValue(2, proj);
    m_prog->setUniformValue(3, (view * m_model).inverted().transposed());

    m_prog->setUniformValue(8, viewPos);

    m_prog->setUniformValue(10, m_material.ambient);
    m_prog->setUniformValue(11, m_material.diffuse);
    m_prog->setUniformValue(12, m_material.specular);
    m_prog->setUniformValue(13, m_material.shininess);

    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}
