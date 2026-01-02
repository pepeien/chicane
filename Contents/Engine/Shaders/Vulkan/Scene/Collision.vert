#version 450

struct MeshData {
    mat4 modelMatrix;
    mat4 boundsMatrix;
    uint textureIndex;
};

layout(set = 0, binding = 0) uniform CameraData {
    vec4 clip;

    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;

    vec4 translation;
} camera;

layout(std140, set = 0, binding = 1) readonly buffer StorageBuffer {
    MeshData data[];
} meshes;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

void main() {
    MeshData mesh = meshes.data[gl_InstanceIndex];

    gl_Position = camera.viewProjection * mesh.boundsMatrix * vec4(inPosition, 1.0);
}