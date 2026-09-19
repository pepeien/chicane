import CameraInstance;
import Poly3D;

[vk::binding(0, 0)] ConstantBuffer<CameraInstance> gCamera;
[vk::binding(2, 0)] StructuredBuffer<PolyInstance3D> gMeshes;

struct OutlinePush
{
    float2 pixelOffset;
    float2 padding;
    float4 color;
}

#ifdef CHICANE_VULKAN
[vk::push_constant]
#else
[vk::binding(15, 0)]
#endif
ConstantBuffer<OutlinePush> push;

[shader("vertex")]
float4 main(
    [vk::location(0)] float3 inPosition : POSITION,
    uint instanceId : SV_InstanceID
) : SV_Position
{
    PolyInstance3D mesh = gMeshes[instanceId];

    float4 clipPosition = mul(gCamera.projection, mul(gCamera.view, mul(mesh.model, float4(inPosition, 1.0))));
    clipPosition.xy += push.pixelOffset * clipPosition.w;

    return clipPosition;
}
