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
[vk::binding(0, 1)] SamplerCube material;

struct VSOut
{
    float3 uv : TEXCOORD0;
}

[shader("fragment")]
float4 main(VSOut input) : SV_Target
{
    float3 sampleColor = material.Sample(input.uv).rgb;

    return float4(sampleColor * push.intensity, 1.0);
}
