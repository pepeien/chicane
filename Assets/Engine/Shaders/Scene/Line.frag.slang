struct VSOut
{
    float4 color : COLOR;
}

[shader("fragment")]
float4 main(VSOut inData) : SV_Target
{
    return inData.color;
}
