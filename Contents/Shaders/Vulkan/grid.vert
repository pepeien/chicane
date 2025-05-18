#version 450

layout(location = 1) out vec4 outColor;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

layout(push_constant) uniform constants
{
    vec2 size;
    vec2 position;
} PushConstants;

void main() {
    const vec2 scaledPos = inPosition.xy * PushConstants.size;
    const vec2 finalPos  = scaledPos + PushConstants.position;

    outColor = inColor;

    gl_Position = vec4(finalPos, 0.0, 1.0);
}