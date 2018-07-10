#version 430 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 vFragPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vTangentLightPos;
layout (location = 3) in vec3 vTangentViewPos;
layout (location = 4) in vec3 vTangentFragPos;

layout (location = 0) out vec4 FragColor;

layout (location = 4) uniform sampler2D diffuseMap;
layout (location = 5) uniform sampler2D normalMap;
layout (location = 6) uniform sampler2D specularMap;

layout (location = 8) uniform vec3 lightPos;
layout (location = 9) uniform vec3 viewPos;

void main()
{
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, vUV).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    // get diffuse color
    vec3 color = texture(diffuseMap, vUV).rgb;
    // ambient
    vec3 ambient = 0.5 * color;
    // diffuse
    vec3 lightDir = normalize(vTangentLightPos - vTangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(vTangentViewPos - vTangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, reflectDir), 0.0), 8.0);

    vec3 specular = texture(specularMap, vUV).rgb * spec * color;
    //FragColor = vec4(ambient + diffuse + specular, 1.0);
    FragColor = vec4(ambient * 2, 1.0);
}
