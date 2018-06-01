#version 430 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

layout (location = 0) in vec3 vFragPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

layout (location = 0) out vec4 fragColor;

layout (location = 1) uniform mat4 uNormalMat;
layout (location = 7) uniform sampler2D tex0;
layout (location = 8) uniform vec3 uViewPos;
layout (location = 9) uniform vec3 uLightPos;
layout (location = 10) uniform Material uMaterial;

// Calculate phong illumination value.
vec3 calcPhongLight(vec3 viewDir, vec3 lightDir) {

    vec3 normal = normalize(vec4(uNormalMat * vec4(vNormal, 0.0)).xyz);

    // Ambient light value calculation.
    vec3 ambient = uMaterial.ambient;

    length(normal);
    // Diffuse light value calculation.
    vec3 diffuse = max(dot(normal, lightDir), 0.0f) * uMaterial.diffuse;

    // Specular light value calculation.
    vec3 specular = pow(max(
                            dot(viewDir, reflect(-lightDir, normal)),
                            0.0f),
                        uMaterial.shininess)
                    * uMaterial.specular;

    float distance = distance(vFragPos, uLightPos);
    float attenuation = 1.0f + 0.045f * distance + 0.0075f * distance * distance;
    float luminosity = 1.0f / attenuation;

    return(vec3(ambient + diffuse + specular) * luminosity);
}

void main() {
    vec4 texColor = texture(tex0, vUV);

    // Calculate viewDir & lightDir and normalize.
    vec3 viewDir = normalize(uViewPos - vFragPos);
    vec3 lightDir = normalize(uLightPos - vFragPos);

    // Calculate phong illumination: vec3 phongLight.
    vec3 phongLight = calcPhongLight(viewDir, lightDir);

    fragColor = vec4((phongLight), 1.0f);
}
