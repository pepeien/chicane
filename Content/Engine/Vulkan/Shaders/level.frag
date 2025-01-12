#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 1, binding = 0) uniform sampler2D material[1000];

layout(location = 0) out vec4 outColor;

layout(location = 0) in flat int inTextureIndex;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexturePosition;
layout(location = 3) in vec3 inNormalPosition;

const vec4 lightColor     = vec4(0.7, 0.7, 0.7, 1.0);
const vec3 lightDirection = normalize(vec3(1.0, 1.0, -1.0));

void main() {
    float angle = clamp(dot(inNormalPosition, -lightDirection.xyz), 0.0f, 1.0f);
    vec3 color  = lightColor.xyz * angle;

    outColor = vec4(color, 1.0f) * texture(material[inTextureIndex], inTexturePosition);
}