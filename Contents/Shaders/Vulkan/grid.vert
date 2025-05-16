#version 450

struct MeshData {
    vec2 size;
    vec2 position;
};

layout(std140, set = 0, binding = 0) readonly buffer StorageBuffer {
    MeshData data[];
} meshes;

layout(location = 1) out vec4 outColor;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

void main() {
    MeshData mesh = meshes.data[gl_InstanceIndex];

    outColor = inColor;

    vec2 scaledPos = inPosition.xy * mesh.size;
    vec2 finalPos  = scaledPos + mesh.position;

    gl_Position = vec4(finalPos, 0.0, 1.0);
}