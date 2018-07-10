#version 430 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (location = 0) out vec3 vFragPos;
layout (location = 1) out vec2 vUV;
layout (location = 2) out vec3 vTangentViewPos;
layout (location = 3) out vec3 vTangentFragPos;
layout (location = 4) out vec3 vTangentLightPos;
layout (location = 5) out vec3 vTangentLightPos2;

layout (location = 0) uniform mat4 uModelMat;
layout (location = 1) uniform mat4 uViewMat;
layout (location = 2) uniform mat4 uProjectionMat;
layout (location = 3) uniform mat4 uNormalMat;

layout (location = 9) uniform vec3 viewPos;
layout (location = 10) uniform vec3 lightPos;
layout (location = 11) uniform vec3 lightPos2;

void main() {
    vFragPos = vec3(uModelMat * vec4(aPosition, 1.0));
    vUV = aUV;

    mat3 normalMatrix = mat3(uNormalMat);
    vec3 tangent = normalize(normalMatrix * aTangent);
    vec3 normal = normalize(normalMatrix * aNormal);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(normal, tangent);

    mat3 TBN = transpose(mat3(tangent, bitangent, normal));
    vTangentLightPos = TBN * lightPos;
    vTangentLightPos2 = TBN * lightPos2;
    vTangentViewPos  = TBN * viewPos;
    vTangentFragPos  = TBN * vFragPos;

    gl_Position = uProjectionMat * uViewMat * uModelMat * vec4(aPosition, 1.0);
}
