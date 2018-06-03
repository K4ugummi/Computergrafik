#version 430 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

#define NUM_LIGHTS 5

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;

    vec3 color;

    float ambient;
    float diffuse;
    float specular;

    float shininess;
    float constant;
    float linear;
    float quadratic;
};

layout (location = 0) in vec3 vFragPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

layout (location = 7) uniform sampler2D tex0;
layout (location = 8) uniform vec3 uViewPos;
layout (location = 10) uniform Material uMaterial;

layout (std140) uniform lightBlock {
    PointLight lights[5];
};

// Calculate phong illumination value.
vec3 calcPhongLight(PointLight light, vec3 viewDir, vec3 lightDir) {

    // Ambient light value calculation.
    vec3 ambient = uMaterial.ambient;// * light.ambient;

    // Diffuse light value calculation.
    vec3 diffuse = max(dot(vNormal, lightDir), 0.0f) * uMaterial.diffuse;// * light.diffuse;

    // Specular light value calculation.
    vec3 reflectDir = reflect(-lightDir, vNormal);
    vec3 specular = pow(max(
                            dot(viewDir, reflectDir),
                            0.0f),
                        uMaterial.shininess)
                    * uMaterial.specular;// * light.specular;

    float distance = distance(vFragPos, light.position);
    float attenuation =
            light.constant
            + light.linear * distance
            + light.quadratic * distance * distance;
    float luminosity = 1.0f / attenuation;

    return((ambient + diffuse + specular) * light.color);
}

void main() {
    vec4 texColor = texture(tex0, vUV);

    vec3 viewDir = normalize(uViewPos - vFragPos);

    // Calculate phong illumination for every light in UBO.
    vec3 phongLighting = vec3(0.0f);
    //for (int i = 0; i < 5; i++) {
        vec3 lightDir = normalize(lights[2].position - vFragPos);
        phongLighting += calcPhongLight(lights[2], viewDir, lightDir);
    //}

    gl_FragColor = vec4(phongLighting, 1.0f);
}
