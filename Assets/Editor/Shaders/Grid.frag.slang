import CameraInstance;
import Grid;

[vk::binding(0, 0)] ConstantBuffer<CameraInstance> gCamera;

struct VSOut
{
    float3 nearPoint : TEXCOORD0;
    float3 farPoint : TEXCOORD1;
}

struct FSOut
{
    float4 color : SV_Target;
    float depth : SV_Depth;
}

[shader("fragment")]
FSOut main(VSOut inData)
{
    float floorDistance = -inData.nearPoint.z / (inData.farPoint.z - inData.nearPoint.z);

    if (floorDistance < 0.0)
    {
        discard;
    }

    float3 position      = inData.nearPoint + floorDistance * (inData.farPoint - inData.nearPoint);
    float4 worldPosition = mul(gCamera.projection, mul(gCamera.view, float4(position, 1.0)));

    FSOut output;
    output.color = GridLayer(position, gCamera.translation.xyz, ComputeGridLinearDepth(worldPosition, gCamera.clip.x, gCamera.clip.y));
    output.depth = ComputeGridDepth(worldPosition);

    return output;
}
