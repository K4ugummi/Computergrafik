// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#ifndef MY_MESH_H
#define MY_MESH_H

#include "modelloaderh.h"

#include <vector>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include <QImage>

struct Vertex {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat uv[2];
};

class Mesh : QOpenGLFunctions_3_3_Core {
private:
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    GLuint m_vao;   // Vertex Array Object
    GLuint m_vbo;   // Vertex Buffer Object
    GLuint m_ibo;   // Index Buffer Object
    GLuint m_tex;   // Texture

    QVector3D m_color;

    QMatrix4x4 m_model;

    QOpenGLShaderProgram * m_prog;  // Program to render this Mesh.

protected:
public:
    Mesh(QString filepath);
    ~Mesh();

    void setProgram(QOpenGLShaderProgram * prog);
    void setColor(QVector3D color);

    QVector3D getColor();
    QMatrix4x4 getModel();

    void bindProgram();

    void scale(GLfloat scale);
    void rotate(GLfloat angle, QVector3D axis);
    void rotateUV(GLfloat angle, QVector2D point);
    void translate(QVector3D translate);

    void draw();
};

#endif // MY_MESH_H
