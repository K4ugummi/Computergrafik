#version 430 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 vFragPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vTangentViewPos;
layout (location = 3) in vec3 vTangentFragPos;
layout (location = 4) in vec3 vTangentLightPos;
layout (location = 5) in vec3 vTangentLightPos2;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D diffuseMap;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D specularMap;
layout (binding = 3) uniform sampler2D emissionMap;

layout (location = 9) uniform vec3 viewPos;

layout (location = 10) uniform vec3 lightPos;
layout (location = 11) uniform vec3 lightPos2;

void main() {
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, vUV).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    // get diffuse color
    vec3 color = texture(diffuseMap, vUV).rgb;

    // ambient
    vec3 ambient = 0.1 * color;

    // diffuse
    vec3 lightDir = normalize(vTangentLightPos - vTangentFragPos);
    vec3 lightDir2 = normalize(vTangentLightPos2 - vTangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    float diff2 = max(dot(lightDir2, normal), 0.0);
    vec3 diffuse = (diff + diff2) * color;

    // specular
    vec3 viewDir = normalize(vTangentViewPos - vTangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 halfwayDir2 = normalize(lightDir2 + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    float spec2 = pow(max(dot(normal, halfwayDir2), 0.0), 16.0);
    vec3 specular = texture(specularMap, vUV).rgb * (spec + spec2);

    // emission
    vec3 emission = texture(emissionMap, vUV).rgb;

    //FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
    FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}
