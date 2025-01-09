#version 450

layout(set = 0, binding = 0) uniform CameraUBO {
    vec4 clip;

    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;
} camera;

struct MeshData {
    mat4 transform;
    vec4 textureIndex;
};

layout(std140, set = 0, binding = 1) readonly buffer StorageBuffer {
    MeshData data[];
} model;

layout(location = 0) out flat int outTextureIndex;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec2 outTexturePosition;
layout(location = 3) out vec3 outNormalPosition;

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec3 inVertexColor;
layout(location = 2) in vec2 inVertexTexturePosition;
layout(location = 3) in vec3 inVertexNormalPosition;

void main() {
    MeshData data  = model.data[gl_InstanceIndex];
    vec4 position  = vec4(inVertexPosition.xyz, 1.0);

    mat4 pvm = camera.viewProjection * data.transform;

    gl_Position = pvm * position;

    outTextureIndex    = int(data.textureIndex.x);
    outColor           = inVertexColor;
    outTexturePosition = inVertexTexturePosition;
    outNormalPosition  = normalize((data.transform * vec4(inVertexNormalPosition, 0.0)).xyz);
}