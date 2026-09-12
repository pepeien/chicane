#ifndef CHICANE_TEXTURE_GLSL
#define CHICANE_TEXTURE_GLSL

layout(std430, binding = 5) readonly buffer TextureTable {
    uvec2 slots[];
};

layout(binding = 6)  uniform sampler2DArray textures0;
layout(binding = 7)  uniform sampler2DArray textures1;
layout(binding = 8)  uniform sampler2DArray textures2;
layout(binding = 9)  uniform sampler2DArray textures3;
layout(binding = 10) uniform sampler2DArray textures4;
layout(binding = 11) uniform sampler2DArray textures5;

vec4 sampleTextureSlot(int index, vec2 uv, float lodBias) {
    if (index < 0) {
        return vec4(1.0);
    }

    uvec2 slot  = slots[uint(index)];
    vec3  coord = vec3(uv, float(slot.y));
    if (slot.x == 1u) {
        return texture(textures1, coord, lodBias);
    }
    if (slot.x == 2u) {
        return texture(textures2, coord, lodBias);
    }
    if (slot.x == 3u) {
        return texture(textures3, coord, lodBias);
    }
    if (slot.x == 4u) {
        return texture(textures4, coord, lodBias);
    }
    if (slot.x == 5u) {
        return texture(textures5, coord, lodBias);
    }

    return texture(textures0, coord, lodBias);
}

vec4 sampleTextureSlot(int index, vec2 uv) {
    return sampleTextureSlot(index, uv, 0.0);
}

#endif
