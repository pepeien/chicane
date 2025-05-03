#version 450

layout(set = 0, binding = 0) uniform CameraUBO {
    vec4 clip;

    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;

    vec4 translation;
} camera;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 outUV;

void main() {
    outUV     = inPosition;
    outUV.xy *= -1.0;

    gl_Position = (mat4(mat3(camera.viewProjection)) * vec4(inPosition, 1.0)).xyzz;
}