import Tonemap;

struct HdrPass
{
    int enabled;
    int bloom;
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
    float4 hdrSample = hdrColor.Sample(input.uv);
    float3 hdr       = hdrSample.rgb;
    float  alpha     = saturate(hdrSample.a);
    float3 bloom     = float3(0.0, 0.0, 0.0);
    if (hdrPass.bloom != 0 && alpha > 0.0)
    {
        bloom = bloomColor.Sample(input.uv).rgb * 0.85 * alpha;
    }
    if (hdrPass.enabled == 0)
    {
        return float4(saturate(hdr) + bloom, alpha);
    }

    return float4(acesTonemap(hdr + bloom), alpha);
}
