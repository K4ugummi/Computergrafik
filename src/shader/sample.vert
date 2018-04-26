#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec3 vColor;

void main() {
    vColor = aColor;

    gl_Position = vec4(aPosition, 0.0f, 1.0f);
}
