import CameraInstance;
import Particle;

[vk::binding(0, 0)] ConstantBuffer<CameraInstance> gCamera;
[vk::binding(1, 0)] StructuredBuffer<GpuParticle> gParticles;

struct VSOut
{
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    float additive : TEXCOORD1;
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
VSOut main(uint vertexId : SV_VertexID, uint instanceId : SV_InstanceID)
{
    GpuParticle particle = gParticles[instanceId];

    float3 position = particle.positionRotation.xyz;
    float rotation  = particle.positionRotation.w;
    float2 size     = particle.sizeAge.xy;
    float3 axis     = particle.axis.xyz;

    float2 corner = quadCorner(vertexId);
    float2 uv     = corner + float2(0.5, 0.5);

    float3 offset = float3(0.0, 0.0, 0.0);
    if (dot(axis, axis) > 0.0001)
    {
        float3 along    = normalize(axis);
        float3 toCamera = gCamera.translation.xyz - position;
        float3 side     = cross(along, toCamera);
        if (dot(side, side) < 0.0001)
        {
            side = gCamera.right.xyz;
        }
        side   = normalize(side);
        offset = along * (corner.x * size.x) + side * (corner.y * size.y);
    }
    else
    {
        float cosine   = cos(rotation);
        float sine     = sin(rotation);
        float2 rotated = float2(corner.x * cosine - corner.y * sine, corner.x * sine + corner.y * cosine);
        offset         = gCamera.right.xyz * (rotated.x * size.x) + gCamera.up.xyz * (rotated.y * size.y);
    }

    VSOut output;
    output.uv       = uv;
    output.color    = particle.color;
    output.additive = particle.sizeAge.w;
    output.position = mul(gCamera.projection, mul(gCamera.view, float4(position + offset, 1.0)));

    return output;
}
