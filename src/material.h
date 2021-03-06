#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector3D>

struct Material {
    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;
    float shininess;
};

namespace Materials {

const Material Emerald = Material{ { 0.0215, 0.1745, 0.0215 }, { 0.07568, 0.61424, 0.07568 }, { 0.633, 0.727811, 0.633 }, 0.6, };
const Material Jade = Material{ { 0.135, 0.2225, 0.1575 }, { 0.54, 0.89, 0.63 }, { 0.316228, 0.316228, 0.316228 }, 0.1, };
const Material Obsidian = Material{ { 0.05375, 0.05, 0.06625 }, { 0.18275, 0.17, 0.22525 }, { 0.332741, 0.328634, 0.346435 }, 0.3, };
const Material Pearl = Material{ { 0.25, 0.20725, 0.20725 }, { 1.0, 0.829, 0.829 }, { 0.296648, 0.296648, 0.296648 }, 0.088, };
const Material Ruby = Material{ { 0.1745, 0.01175, 0.01175 }, { 0.61424, 0.04136, 0.04136 }, { 0.727811, 0.626959, 0.626959 }, 0.6, };
const Material Turquoise = Material{ { 0.1, 0.18725, 0.1745 }, { 0.396, 0.74151, 0.69102 }, { 0.297254, 0.30829, 0.306678 }, 0.1, };
const Material Brass = Material{ { 0.329412, 0.223529, 0.027451 }, { 0.780392, 0.568627, 0.113725 }, { 0.992157, 0.941176, 0.807843 }, 0.21794872, };
const Material Bronze = Material{ { 0.2125, 0.1275, 0.054 }, { 0.714, 0.4284, 0.18144 }, { 0.393548, 0.271906, 0.166721 }, 0.2, };
const Material Chrome = Material{ { 0.25, 0.25, 0.25 }, { 0.4, 0.4, 0.4 }, { 0.774597, 0.774597, 0.774597 }, 0.6, };
const Material Copper = Material{ { 0.19125, 0.0735, 0.0225 }, { 0.7038, 0.27048, 0.0828 }, { 0.256777, 0.137622, 0.086014 }, 0.1, };
const Material Gold = Material{ { 0.24725, 0.1995, 0.0745 }, { 0.75164, 0.60648, 0.22648 }, { 0.628281, 0.555802, 0.366065 }, 0.4, };
const Material Silver = Material{ { 0.19225, 0.19225, 0.19225 }, { 0.50754, 0.50754, 0.50754 }, { 0.508273, 0.508273, 0.508273 }, 0.4, };
const Material PlasticBlack = Material{ { 0.0, 0.0, 0.0 }, { 0.01, 0.01, 0.01 }, { 0.50, 0.50, 0.50 }, .25, };
const Material PlasticCyan = Material{ { 0.0, 0.1, 0.06 }, { 0.0, 0.50980392, 0.50980392 }, { 0.50196078, 0.50196078, 0.50196078 }, .25, };
const Material PlasticGreen = Material{ { 0.0, 0.0, 0.0 }, { 0.1, 0.35, 0.1 }, { 0.45, 0.55, 0.45 }, .25, };
const Material PlasticRed = Material{ { 0.0, 0.0, 0.0 }, { 0.5, 0.0, 0.0 }, { 0.7, 0.6, 0.6 }, .25, };
const Material PlasticWhite = Material{ { 0.0, 0.0, 0.0 }, { 0.55, 0.55, 0.55 }, { 0.70, 0.70, 0.70 }, .25, };
const Material PlasticYellow = Material{ { 0.0, 0.0, 0.0 }, { 0.5, 0.5, 0.0 }, { 0.60, 0.60, 0.50 }, .25, };
const Material RubberBlack = Material{ { 0.02, 0.02, 0.02 }, { 0.01, 0.01, 0.01 }, { 0.4, 0.4, 0.4 }, .078125, };
const Material RubberCyan = Material{ { 0.0, 0.05, 0.05 }, { 0.4, 0.5, 0.5 }, { 0.04, 0.7, 0.7 }, .078125, };
const Material RubberGreen = Material{ { 0.0, 0.05, 0.0 }, { 0.4, 0.5, 0.4 }, { 0.04, 0.7, 0.04 }, .078125, };
const Material RubberRed = Material{ { 0.05, 0.0, 0.0 }, { 0.5, 0.4, 0.4 }, { 0.7, 0.04, 0.04 }, .078125, };
const Material RubberWhite = Material{ { 0.05, 0.05, 0.05 }, { 0.5, 0.5, 0.5 }, { 0.7, 0.7, 0.7 }, .078125, };
const Material RubberYellow = Material{ { 0.05, 0.05, 0.0 }, { 0.5, 0.5, 0.4 }, { 0.7, 0.7, 0.04 }, .078125, };

} // Materials

namespace IOR {

// Gases
const float Air = 1.0;
const float Gas = 1.0;

// Liquids
const float Water = 1.333;
const float Ethanol = 1.36;
const float OliveOil = 1.47;

// Solids
const float Ice = 1.31;
const float PMMA = 1.49;
const float WindowGlass = 1.52;
const float Polycarbonate = 1.58;
const float FlintGlass = 1.62;
const float Sapphire = 1.77;
const float Diamond = 2.42;

} // IOR

#endif // MATERIAL_H
