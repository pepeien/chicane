module Texture;

#ifdef CHICANE_VULKAN
[vk::binding(16, 1)] Sampler2D textures[];

public float4 sampleTextureSlot(int index, float2 uv, float lodBias)
{
    if (index < 0)
    {
        return float4(1.0);
    }

    return textures[NonUniformResourceIndex(index)].SampleBias(uv, lodBias);
}
#else
[vk::binding(5, 0)] RWStructuredBuffer<uint4> slots;
[vk::binding(6, 0)] Sampler2DArray textures0;
[vk::binding(7, 0)] Sampler2DArray textures1;
[vk::binding(8, 0)] Sampler2DArray textures2;
[vk::binding(9, 0)] Sampler2DArray textures3;
[vk::binding(10, 0)] Sampler2DArray textures4;
[vk::binding(11, 0)] Sampler2DArray textures5;

static const uint kTextureClassSizes[6] = {128, 256, 512, 1024, 2048, 4096};

float2 wrapSlotUv(float2 uv)
{
    float2 bInside = step(0.0, uv) * step(uv, 1.0);

    return lerp(frac(uv), uv, bInside);
}

public float4 sampleTextureSlot(int index, float2 uv, float lodBias)
{
    float2 uvGradX = ddx(uv);
    float2 uvGradY = ddy(uv);

    if (index < 0)
    {
        return float4(1.0);
    }

    uint4 slot = slots[uint(index)];
    float classSize = float(kTextureClassSizes[min(slot.x, 5u)]);
    float2 size     = float2(float(max(slot.z, 1u)), float(max(slot.w, 1u)));
    float2 scale    = size / classSize;
    float3 coord    = float3(wrapSlotUv(uv) * scale, float(slot.y));
    float2 gradX    = uvGradX * scale * exp2(lodBias);
    float2 gradY    = uvGradY * scale * exp2(lodBias);
    if (slot.x == 1u)
    {
        return textures1.SampleGrad(coord, gradX, gradY);
    }
    if (slot.x == 2u)
    {
        return textures2.SampleGrad(coord, gradX, gradY);
    }
    if (slot.x == 3u)
    {
        return textures3.SampleGrad(coord, gradX, gradY);
    }
    if (slot.x == 4u)
    {
        return textures4.SampleGrad(coord, gradX, gradY);
    }
    if (slot.x == 5u)
    {
        return textures5.SampleGrad(coord, gradX, gradY);
    }

    return textures0.SampleGrad(coord, gradX, gradY);
}
#endif

public float4 sampleTextureSlot(int index, float2 uv)
{
    return sampleTextureSlot(index, uv, 0.0);
}
