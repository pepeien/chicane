import CameraInstance;

[vk::binding(0, 0)] ConstantBuffer<CameraInstance> gCamera;

struct IconPush
{
    float4 positionSize;
    float4 color;
    int4 extra;
}

#ifdef CHICANE_VULKAN
[vk::push_constant]
#else
[vk::binding(15, 0)]
#endif
ConstantBuffer<IconPush> push;

struct VSOut
{
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    nointerpolation int textureIndex : TEXCOORD1;
    float4 position : SV_Position;
}

float2 quadCorner(uint inIndex)
{
    if (inIndex == 0u)
    {
        return float2(-0.5, -0.5);
    }
    if (inIndex == 1u)
    {
        return float2(0.5, -0.5);
    }
    if (inIndex == 2u)
    {
        return float2(-0.5, 0.5);
    }

    return float2(0.5, 0.5);
}

[shader("vertex")]
VSOut main(uint vertexId : SV_VertexID)
{
    float3 position = push.positionSize.xyz;
    float size      = push.positionSize.w;
    float2 corner   = quadCorner(vertexId);
    float3 offset   = gCamera.right.xyz * (corner.x * size) + gCamera.up.xyz * (corner.y * size);

    VSOut output;
    output.uv           = float2(corner.x + 0.5, 0.5 - corner.y);
    output.color        = push.color;
    output.textureIndex = push.extra.x;
    output.position     = mul(gCamera.projection, mul(gCamera.view, float4(position + offset, 1.0)));

    return output;
}
