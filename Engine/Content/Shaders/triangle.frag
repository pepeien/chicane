#version 460

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexturePosition;
layout(location = 2) in vec3 fragNormalPosition;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D material;

const vec4 lightColor     = vec4(1.0);
const vec3 lightDirection = normalize(vec3(1.0, 1.0, -1.0));

void main() {
    outColor = lightColor * max(0.0, dot(fragNormalPosition, -lightDirection)) * texture(material, fragTexturePosition);
}