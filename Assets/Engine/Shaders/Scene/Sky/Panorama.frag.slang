struct SkyPush
{
    float intensity;
}

#ifdef CHICANE_VULKAN
[vk::push_constant]
#else
[vk::binding(15, 0)]
#endif
ConstantBuffer<SkyPush> push;
[vk::binding(0, 1)] Sampler2D material;

struct VSOut
{
    float3 uv : TEXCOORD0;
}

[shader("fragment")]
float4 main(VSOut input) : SV_Target
{
    float3 direction = normalize(input.uv);
    float  yaw       = atan2(direction.x, direction.y);
    float  pitch     = asin(clamp(direction.z, -1.0, 1.0));
    float2 uv        = float2(yaw * 0.15915494309 + 0.5, 0.5 - pitch * 0.31830988618);
    uv.x             = uv.x - floor(uv.x);
    uv.y             = clamp(uv.y, 0.0, 1.0);

    float3 sampleColor = material.Sample(uv).rgb;

    return float4(sampleColor * push.intensity, 1.0);
}
