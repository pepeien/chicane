#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexturePosition;
layout(location = 2) in vec3 fragNormalPosition;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D material;

const vec4 lightColor     = vec4(1.0);
const vec3 lightDirection = normalize(vec3(1.0, -1.0, -1.0));

void main() {
    float angle = clamp(dot(fragNormalPosition, -lightDirection.xyz), 0.0f, 1.0f);
    vec3 color  = lightColor.xyz * angle;

    outColor = vec4(color, 0.5f) * texture(material, fragTexturePosition);
}