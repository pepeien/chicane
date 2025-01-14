#version 450

layout(set = 0, binding = 0) uniform CameraUBO {
    vec4 clip;

    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    mat4 inversedViewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;

    vec4 translation;
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

    gl_Position = camera.viewProjection * data.transform * vec4(inVertexPosition, 1.0);

    outTextureIndex    = int(data.textureIndex.x);
    outColor           = inVertexColor;
    outTexturePosition = inVertexTexturePosition;

    mat3 normalMatrix  = transpose(inverse(mat3(data.transform)));
    outNormalPosition  = normalize(normalMatrix * inVertexNormalPosition);
}