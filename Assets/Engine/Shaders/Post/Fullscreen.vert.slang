struct VSOut
{
    float2 uv : TEXCOORD0;
    float4 position : SV_Position;
}

[shader("vertex")]
VSOut main(uint vertexId : SV_VertexID)
{
    float2 uv = float2((vertexId << 1) & 2, vertexId & 2);

    VSOut output;
    output.uv       = uv;
    output.position = float4(uv * 2.0 - 1.0, 0.0, 1.0);

    return output;
}
