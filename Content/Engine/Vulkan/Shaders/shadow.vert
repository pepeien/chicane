#version 450

struct MeshData {
    mat4 model;
    vec4 textureIndex;
};

layout(location = 0) out vec4 outOne;

layout(set = 0, binding = 0) uniform LightData {
    vec4 clip;

    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;

    vec4 translation;
} light;

layout(std140, set = 0, binding = 1) readonly buffer StorageBuffer {
    MeshData data[];
} meshes;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

void main() {
    MeshData mesh = meshes.data[gl_InstanceIndex];

    outOne = light.viewProjection * mesh.model * vec4(inPosition, 1.0);

    gl_Position = outOne;
}