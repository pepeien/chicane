import Tonemap;

[vk::binding(0, 0)] Sampler2D hdrColor;

struct VSOut
{
    float2 uv : TEXCOORD0;
}

[shader("fragment")]
float4 main(VSOut input) : SV_Target
{
    float3 color         = hdrColor.Sample(input.uv).rgb;
    float luma           = luminance(color);
    float contribution   = max(luma - 1.0, 0.0);
    float3 bloom         = color * (contribution / max(luma, 0.0001));

    return float4(bloom, 1.0);
}
