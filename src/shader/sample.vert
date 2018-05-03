#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;

layout(location = 0) out vec3 vColor;
layout(location = 1) out vec2 vUV;

layout(location = 1) uniform float offsetX;
layout(location = 2) uniform float offsetY;
layout(location = 3) uniform mat4x4 uRotationMat;

void main() {
    vColor = aColor;

    vUV = vec2(aUV.x + offsetX, aUV.y + offsetY);

    //vColor = texture(tex0, vUV);

    gl_Position = uRotationMat * vec4(aPosition, 0.0f, 1.0f);
}
