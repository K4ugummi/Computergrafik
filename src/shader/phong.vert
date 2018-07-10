#version 430 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

layout (location = 0) out vec3 vFragPos;
layout (location = 1) out vec3 vNormal;
layout (location = 2) out vec2 vUV;
layout (location = 3) out int vIsRender;

layout (location = 0) uniform mat4 uModelMat;
layout (location = 1) uniform mat4 uViewMat;
layout (location = 2) uniform mat4 uProjectionMat;
layout (location = 3) uniform mat4 uNormalMat;

void main() {
    if (aPosition.z > 0.0 && aPosition.z < 1.0)
        vIsRender = 1;
    else
        vIsRender = 0;

    vFragPos = vec3(uViewMat * uModelMat * vec4(aPosition, 1.0f));
    vNormal = normalize(mat3(uNormalMat) * aNormal);
    vUV = aUV;

    gl_Position = uProjectionMat * uViewMat * uModelMat * vec4(aPosition, 1.0f);
}
