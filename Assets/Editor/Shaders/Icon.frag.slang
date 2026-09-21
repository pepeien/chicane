import Texture;

struct VSOut
{
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    nointerpolation int textureIndex : TEXCOORD1;
}

[shader("fragment")]
float4 main(VSOut inData) : SV_Target
{
    float4 sample = sampleTextureSlot(inData.textureIndex, inData.uv);
    float4 color  = sample * inData.color;
    if (color.a < 0.08)
    {
        discard;
    }

    return color;
}
