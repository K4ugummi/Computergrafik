// Computergrafik Praktikum SS2018
// Autor: Stephan Schauerte
// Matrikelnummer: 3099441

#ifndef MY_MESH_H
#define MY_MESH_H

#include "vertex.h"
#include "material.h"
#include "modelloaderh.h"

#include <vector>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_3_Core>
#include <QImage>

class Mesh : QOpenGLFunctions_4_3_Core {
private:
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    GLuint m_vao;   // Vertex Array Object
    GLuint m_vbo;   // Vertex Buffer Object
    GLuint m_ibo;   // Index Buffer Object
    GLuint m_tex;   // Texture

    // Additional Color that is multiplied with the texture.
    // Set to (1.0f, 1.0f, 1.0f) for texture only
    QVector3D m_color;
    Material m_material;

    GLfloat m_scale;        // Scaling factor.
    QVector3D m_position;   // Position in World space.
    QVector3D m_rotation;   // Euler rotation for x-, y-, z-axis.

    QMatrix4x4 m_model; // Model Matrix (Rotation, Scale, Translation)

    QOpenGLShaderProgram * m_prog;  // Program to render this Mesh.

protected:
public:
    Mesh(QString filepath);
    ~Mesh();

    void setProgram(QOpenGLShaderProgram * prog);
    void setColor(QVector3D color);
    void setMaterial(Material mat);
    void setScale(GLfloat scale);
    void setPosition(QVector3D position);
    void setModel(QMatrix4x4 model);
    void setRotation(QMatrix4x4 rotMat);

    QVector3D getColor();
    QMatrix4x4 getModel();
    QVector3D getPosition();

    void scale(GLfloat scale);
    void rotate(GLfloat angle, QVector3D axis);
    void rotate(QQuaternion quat);
    void rotateRawZ(GLfloat angle);
    void translate(QVector3D translate);

    void draw(const QMatrix4x4 &view, const QMatrix4x4 &proj, const QVector3D &viewPos);
};

#endif // MY_MESH_H
