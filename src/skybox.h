// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#ifndef MY_SKYBOX_H
#define MY_SKYBOX_H

#include <vector>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_3_Core>
#include <QImage>

struct SkyBoxVertex {
    GLfloat position[3];
};

class Skybox : QOpenGLFunctions_4_3_Core {
private:
    std::vector<GLfloat> m_vertices;

    std::vector<GLuint> m_indices;

    GLuint m_vao;   // Vertex Array Object
    GLuint m_vbo;   // Vertex Buffer Object
    GLuint m_ibo;   // Index Buffer Object
    GLuint m_cubeTex;   // Texture

    QOpenGLShaderProgram * m_prog;  // Program to render this Skybox.

protected:
public:
    Skybox();
    ~Skybox();

    void draw(const QMatrix4x4 &projection, QMatrix4x4 view);
};

#endif // MY_SKYBOX_H
