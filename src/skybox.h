// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#ifndef MY_SKYBOX_H
#define MY_SKYBOX_H

#include <vector>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include <QImage>

struct SkyBoxVertex {
    GLfloat position[3];
};

class Skybox : QOpenGLFunctions_3_3_Core {
private:
    std::vector<SkyBoxVertex> m_vertices;
    std::vector<GLuint> m_indices;

    GLuint m_vao;   // Vertex Array Object
    GLuint m_vbo;   // Vertex Buffer Object
    GLuint m_ibo;   // Index Buffer Object
    GLuint m_tex;   // Texture

    QOpenGLShaderProgram * m_prog;  // Program to render this Skybox.

protected:
public:
    Skybox(QString filepath);
    ~Skybox();

    void setProgram(QOpenGLShaderProgram * prog);
    void setColor(QVector3D color);

    QMatrix4x4 getModel();

    void bindProgram();

    void draw();
};

#endif // MY_SKYBOX_H
