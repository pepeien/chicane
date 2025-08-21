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

layout(set = 0, binding = 1) uniform LightData {
    vec4 clip;

    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;

    vec4 translation;
} light;

layout(std140, set = 0, binding = 2) readonly buffer StorageBuffer {
    MeshData data[];
} meshes;

layout(location = 0) out flat uint outTextureIndex;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec4 outColor;
layout(location = 4) out vec3 outViewPosition;
layout(location = 5) out vec3 outLightPosition;
layout(location = 8) out vec4 outShadowPosition; 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

void main() {
    MeshData mesh = meshes.data[gl_InstanceIndex];

    vec4 position = vec4(inPosition, 1.0);

    outTextureIndex   = mesh.textureIndex;
    outUV             = inUV;
    outNormal         = mat3(mesh.modelMatrix) * inNormal;
    outColor          = inColor;
    outViewPosition   = -inPosition.xyz;
    outLightPosition  = normalize(light.translation.xyz - inPosition);
    outShadowPosition = (biasMat * light.viewProjection * mesh.modelMatrix) * position;

    gl_Position = camera.viewProjection * mesh.modelMatrix * position;
}