import Target;
import LightInstance;
import Poly3D;

[vk::binding(1, 0)] StructuredBuffer<LightInstance> gLights;
[vk::binding(2, 0)] StructuredBuffer<PolyInstance3D> gMeshes;

struct CascadePush
{
    uint cascade;
}

#ifdef CHICANE_VULKAN
[vk::push_constant]
#else
[vk::binding(15, 0)]
#endif
ConstantBuffer<CascadePush> push;

[shader("vertex")]
float4 main(
    [vk::location(0)] float3 inPosition : POSITION,
    uint instanceId : SV_InstanceID
) : SV_Position
{
    PolyInstance3D mesh = gMeshes[instanceId];
    int cascade         = int(push.cascade);

    return mul(
        gLights[0].projections[cascade],
        mul(gLights[0].views[cascade], mul(mesh.model, float4(inPosition, 1.0)))
    );
}
