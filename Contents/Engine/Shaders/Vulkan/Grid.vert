#version 450

layout(location = 1) out vec4 outColor;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

layout(push_constant) uniform constants
{
    vec2  screen;
    vec2  size;
    vec2  position;
    float zIndex;
} PushConstants;

void main() {
    vec2 position = PushConstants.position / PushConstants.screen;
    position *= 2.0;
    position -= 1.0;
    position += inPosition.xy * (PushConstants.size / PushConstants.screen);

    const float zIndex = clamp(abs((PushConstants.zIndex / 999.9) - 1.0), 0.0, 0.9999);

    outColor = inColor / 255.0;

    gl_Position  = vec4(position, zIndex, 1.0);
}