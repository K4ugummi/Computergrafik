#ifndef LIGHTS_H
#define LIGHTS_H

#define NUM_LIGHTS 2

#include <QVector3D>

struct LightSource {
    QVector3D position;
    float pad0;

    QVector3D color;
    float pad1;

    float kAmbiet;
    float kDiffuse;
    float kSpecular;
    float pad2;

    float constant;
    float linear;
    float quadratic;
    float pad3;
};



#endif // LIGHTS_H
