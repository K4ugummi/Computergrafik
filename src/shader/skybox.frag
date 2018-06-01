#version 430 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 vCubeDir;

layout (location = 0) out vec4 fragColor;

layout (location = 7) uniform samplerCube uSkybox;

void main() {
    fragColor = texture(uSkybox, vCubeDir);
}
