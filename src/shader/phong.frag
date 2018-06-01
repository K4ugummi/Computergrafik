#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

layout(location = 0) in vec3 vFragPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

layout(location = 0) out vec4 fragColor;

layout(location = 7) uniform sampler2D tex0;
layout(location = 8) uniform vec3 uViewPos;
layout(location = 9) uniform vec3 uLightPos;
layout(location = 10) uniform Material uMaterial;

// Calculate phong illumination value.
vec3 calcPhongLight(vec3 viewDir, vec3 lightDir) {

    vec3 normal = normalize(vNormal);

    // Ambient light value calculation.
    vec3 ambient = 0.1f;

    // Diffuse light value calculation.
    vec3 diffuse = dot(lightDir, normal);

    // Specular light value calculation.
    vec3 reflectDir = reflect(lightDir, normal);
    vec3 specular = dot(viewDir, reflectDir);

    return(ambient + diffuse + specular);
}

void main() {
    vec3 texColor = texture(tex0, vUV);

    // Calculate viewDir & lightDir and normalize.
    vec3 viewDir = normalize(uViewPos - vFragPos);
    vec3 lightDir = normalize(uLightPos - vFragPos);

    // Calculate phong illumination: vec3 phongLight.
    vec3 phongLight = calcPhongLight(viewDir, lightDir);

    fragColor = vec4((texColor * phongLight), 1.0f);
}
