struct VSOut
{
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    float additive : TEXCOORD1;
}

[shader("fragment")]
float4 main(VSOut inData) : SV_Target
{
    float2 centered = inData.uv * 2.0 - 1.0;
    float falloff   = saturate(1.0 - dot(centered, centered));
    falloff         = pow(falloff, 1.4);

    float4 color = inData.color;
    color.a *= falloff;
    color.rgb *= lerp(color.a, falloff, saturate(inData.additive));

    return color;
}
