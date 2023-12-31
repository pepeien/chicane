#version 460

layout(set = 0, binding = 0) uniform UniformObject {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
} cameraData;

layout(std140, set = 0, binding = 1) readonly buffer StorageBuffer {
    mat4 transforms[];
} modelData;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexTexturePosition;
layout(location = 3) in vec3 vertexNormalPosition;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexturePosition;
layout(location = 2) out vec3 fragNormalPosition;

void main() {
    gl_Position = cameraData.viewProjection *
                  modelData.transforms[gl_InstanceIndex] *
                  vec4(vertexPosition, 1.0);

    fragColor           = vertexColor;
    fragTexturePosition = vertexTexturePosition;
    fragNormalPosition  = normalize(
        (modelData.transforms[gl_InstanceIndex] * vec4(vertexNormalPosition, 0.0)).xyz
    );
}