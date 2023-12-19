#version 460

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexturePosition;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D material;

void main() {
    //outColor = vec4(fragColor, 1.0) * texture(material, fragTexturePosition);
    outColor = texture(material, fragTexturePosition);
}