#version 450

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;
} camera;

layout(std140, set = 0, binding = 1) readonly buffer StorageBuffer {
    mat4 transforms[];
} model;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexturePosition;
layout(location = 2) out vec3 outNormalPosition;

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec3 inVertexColor;
layout(location = 2) in vec2 inVertexTexturePosition;
layout(location = 3) in vec3 inVertexNormalPosition;

void main() {
    mat4 transform = model.transforms[gl_InstanceIndex];

    gl_Position = camera.viewProjection * transform * vec4(inVertexPosition.xyz, 1.0);

    outColor           = inVertexColor;
    outTexturePosition = inVertexTexturePosition;
    outNormalPosition  = normalize((transform * vec4(inVertexNormalPosition, 0.0)).xyz);
}