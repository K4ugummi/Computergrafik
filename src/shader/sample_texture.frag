#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vColor;
layout(location = 1) in vec2 vUV;

layout(location = 0) out vec4 fragColor;

layout(location = 7) uniform sampler2D tex0;

void main() {
    fragColor = texture(tex0, vUV);
}
