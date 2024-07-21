#version 450

layout(set = 1, binding = 0) uniform sampler2D material;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexturePosition;
layout(location = 2) in vec3 inNormalPosition;

const vec4 lightColor     = vec4(1.0);
const vec3 lightDirection = normalize(vec3(1.0, -1.0, -1.0));

void main() {
    float angle = clamp(dot(inNormalPosition, -lightDirection.xyz), 0.0f, 1.0f);
    vec3 color  = lightColor.xyz * angle;

    outColor = vec4(color, 0.5f) * texture(material, inTexturePosition);
}