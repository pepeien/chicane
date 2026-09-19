struct BlurPush
{
    float2 direction;
}

#ifdef CHICANE_VULKAN
[vk::push_constant]
#else
[vk::binding(15, 0)]
#endif
ConstantBuffer<BlurPush> blur;
[vk::binding(0, 0)] Sampler2D source;

struct VSOut
{
    float2 uv : TEXCOORD0;
}

[shader("fragment")]
float4 main(VSOut input) : SV_Target
{
    uint width = 1;
    uint height = 1;
    source.GetDimensions(width, height);
    float2 texel = blur.direction / float2(width, height);

    float3 color = source.Sample(input.uv).rgb * 0.227027;
    color += source.Sample(input.uv + texel).rgb * 0.1945946;
    color += source.Sample(input.uv - texel).rgb * 0.1945946;
    color += source.Sample(input.uv + texel * 2.0).rgb * 0.1216216;
    color += source.Sample(input.uv - texel * 2.0).rgb * 0.1216216;
    color += source.Sample(input.uv + texel * 3.0).rgb * 0.054054;
    color += source.Sample(input.uv - texel * 3.0).rgb * 0.054054;

    return float4(color, 1.0);
}
