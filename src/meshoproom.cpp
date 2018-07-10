// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#include "meshoproom.h"
#include "modelloaderh.h"

#define OFS(s, a) reinterpret_cast<void* const>(offsetof(s, a))

MeshOPRoom::MeshOPRoom(QString objectName) {
    bool success = initializeOpenGLFunctions();
    Q_ASSERT(success);
    Q_UNUSED(success);

    m_model = QMatrix4x4();
    m_scale = 1.0f;
    m_position = QVector3D(0.0f, 0.0f, 0.0f);
    m_rotation = QVector3D(0.0f, 0.0f, 0.0f);
    m_color = QVector3D(1.0f, 1.0f, 1.0f);

    ModelLoader modelloader;
    if (!modelloader.loadObjectFromFile(":/objects/" + objectName + ".obj")) {
        qWarning("Could not load obj");
    }
    std::vector<GLfloat> vbo;
    vbo.reserve(modelloader.lengthOfVBO());
    modelloader.genVBO(vbo.data());

    for (uint i = 0; i < modelloader.lengthOfVBO(); i += 14 ) {
        uint n = i;
        m_vertices.push_back(
                    Vertex {
                        vbo[n++], vbo[n++], vbo[n++], // position
                        vbo[n++], vbo[n++], vbo[n++], // normal
                        vbo[n++], vbo[n++],           // uv
                        vbo[n++], vbo[n++], vbo[n++], // tangent
                        vbo[n++], vbo[n++], vbo[n++], // bitangent
                    } );
    }

    std::vector<GLuint> ibo;
    ibo.reserve(modelloader.lengthOfIndexArray());
    modelloader.genIndexArray(ibo.data());
    for (uint i = 0; i < modelloader.lengthOfIndexArray(); i++) {
        m_indices.push_back(ibo[i]);
    }

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

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFS(Vertex, bitangent));

    // TEX BIND
    QImage img_diffuse, img_specular, img_normal, img_emission;

    img_diffuse.load("../Computergrafik/src/textures/" + objectName + "_diffuse_2k.jpg");
    Q_ASSERT(!img_diffuse.isNull());
    img_diffuse = img_diffuse.mirrored(false, true);

    img_specular.load("../Computergrafik/src/textures/" + objectName + "_specular_2k.jpg");
    Q_ASSERT(!img_diffuse.isNull());
    img_specular = img_specular.mirrored(false, true);

    img_normal.load("../Computergrafik/src/textures/" + objectName + "_normal_2k.jpg");
    Q_ASSERT(!img_diffuse.isNull());
    img_normal = img_normal.mirrored(false, true);

    img_emission.load("../Computergrafik/src/textures/" + objectName + "_emission_2k.jpg");
    Q_ASSERT(!img_emission.isNull());
    img_emission = img_emission.mirrored(false, true);

    glGenTextures(1, &m_tex_diffuse);
    glBindTexture(GL_TEXTURE_2D, m_tex_diffuse);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img_diffuse.width(), img_diffuse.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img_diffuse.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenTextures(1, &m_tex_specular);
    glBindTexture(GL_TEXTURE_2D, m_tex_specular);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img_specular.width(), img_specular.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img_specular.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenTextures(1, &m_tex_normal);
    glBindTexture(GL_TEXTURE_2D, m_tex_normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img_normal.width(), img_normal.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img_normal.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenTextures(1, &m_tex_emission);
    glBindTexture(GL_TEXTURE_2D, m_tex_emission);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img_emission.width(), img_emission.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img_emission.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // VAO UNBIND
    glBindVertexArray(0);
}

MeshOPRoom::~MeshOPRoom() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteTextures(1, &m_tex_diffuse);
    glDeleteTextures(1, &m_tex_specular);
    glDeleteTextures(1, &m_tex_normal);
    glDeleteTextures(1, &m_tex_emission);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
}

void MeshOPRoom::setProgram(QOpenGLShaderProgram * prog) {
    m_prog = prog;
}

void MeshOPRoom::setColor(QVector3D color) {
    m_color = color;
}

void MeshOPRoom::setMaterial(Material mat) {
    m_material = mat;
}

void MeshOPRoom::setModel(QMatrix4x4 model) {
    m_model = model;
}

void MeshOPRoom::setRotation(QMatrix4x4 rotMat) {
    m_model = rotMat;
    m_model.translate(m_position);
    m_model.scale(m_scale);
}

QVector3D MeshOPRoom::getColor() {
    return m_color;
}

QMatrix4x4 MeshOPRoom::getModel() {
    return m_model;
}

QVector3D MeshOPRoom::getPosition() {
    return m_model.column(3).toVector3D();
}

void MeshOPRoom::scale(GLfloat scale) {
    m_model.scale(scale);
}

void MeshOPRoom::rotate(GLfloat angle, QVector3D axis) {
    m_model.rotate(angle, axis);
}

void MeshOPRoom::rotate(QQuaternion quat) {
    m_model.rotate(quat);
}

void MeshOPRoom::setScale(GLfloat scale) {
    m_scale = scale;
    QMatrix4x4 model;
    model.rotate(m_rotation.x(), QVector3D(1,0,0));
    model.rotate(m_rotation.y(), QVector3D(0,1,0));
    model.rotate(m_rotation.z(), QVector3D(0,0,1));
    model.translate(m_position);
    model.scale(scale);
    m_model = model;
}

void MeshOPRoom::setPosition(QVector3D position) {
    m_model.setColumn(3, QVector4D(position, 1.0f));
}

// I tried rotating the uv to align these small black arrows with
// the rotation axis of the next gimbal element.
void MeshOPRoom::rotateRawZ(GLfloat angle) {
    for (uint i = 0; i < m_vertices.size(); i++) {
        float x = m_vertices[i].position[0];
        float y = m_vertices[i].position[1];

        m_vertices[i].position[0] = x * cos(angle) - y * sin(angle);
        m_vertices[i].position[1] = x * sin(angle) + y * cos(angle);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), &m_vertices.front(), GL_STATIC_DRAW);
}

void MeshOPRoom::translate(QVector3D translate) {
    m_model.translate(translate);
}

void MeshOPRoom::draw(const QMatrix4x4 &view, const QMatrix4x4 &proj, const QVector3D &viewPos) {
    Q_ASSERT(m_prog->isLinked());

    glBindVertexArray(m_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_tex_normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_tex_specular);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_tex_emission);

    m_prog->bind();
    m_prog->setUniformValue(0, m_model);
    m_prog->setUniformValue(1, view);
    m_prog->setUniformValue(2, proj);
    m_prog->setUniformValue(3, m_model.inverted().transposed());

    m_prog->setUniformValue(9, viewPos);

    // Light 1
    m_prog->setUniformValue(10, QVector3D(1.0,1.71,1.67));
    // Light 2
    m_prog->setUniformValue(11, QVector3D(1.55,2.0,-1.22));


    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}
