#version 450

struct MeshData {
    mat4 model;
    vec4 textureIndex;
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

layout(location = 0) out flat int outTextureIndex;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outColor;
layout(location = 3) out vec3 outView;
layout(location = 4) out vec3 outLight;
layout(location = 5) out vec4 outShadow;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
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

    gl_Position = camera.viewProjection * mesh.model * vec4(inPosition, 1.0);

    vec4 posistion = mesh.model * vec4(inPosition, 1.0);

    outTextureIndex = int(mesh.textureIndex.x);
    outNormal       = mat3(mesh.model) * inNormal;
    outColor        = inColor;
    outLight        = normalize(light.translation.xyz - inPosition);
    outView         = -posistion.xyz;
    outShadow       = (biasMat * light.viewProjection * mesh.model) * vec4(inPosition, 1.0);	
}