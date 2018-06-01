#ifndef VERTEX_H
#define VERTEX_H

#include <QOpenGLFunctions_3_3_Core>

struct Vertex {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat uv[2];
};

#endif // VERTEX_H
