#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

layout (location = 0) out vec3 vFragPos;
layout (location = 1) out vec3 vNormal;
layout (location = 2) out vec2 vUV;

layout (location = 0) uniform mat4 uMVP;

void main() {

    vFragPos = aPosition;
    vNormal = normalize(aNormal);
    vUV = aUV;

    gl_Position = uMVP * vec4(aPosition, 1.0f);
}
