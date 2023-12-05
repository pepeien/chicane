#version 460

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}