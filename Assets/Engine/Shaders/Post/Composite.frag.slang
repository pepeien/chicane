import Tonemap;

struct HdrPass
{
    int enabled;
}

#ifdef CHICANE_VULKAN
[vk::push_constant]
#else
[vk::binding(15, 0)]
#endif
ConstantBuffer<HdrPass> hdrPass;
[vk::binding(0, 0)] Sampler2D hdrColor;
[vk::binding(1, 0)] Sampler2D bloomColor;

struct VSOut
{
    float2 uv : TEXCOORD0;
}

[shader("fragment")]
float4 main(VSOut input) : SV_Target
{
    float3 hdr   = hdrColor.Sample(input.uv).rgb;
    float3 bloom = bloomColor.Sample(input.uv).rgb * 0.85;
    if (hdrPass.enabled == 0)
    {
        return float4(saturate(hdr) + bloom, 1.0);
    }

    return float4(acesTonemap(hdr + bloom), 1.0);
}
