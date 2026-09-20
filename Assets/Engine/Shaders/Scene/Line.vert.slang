import CameraInstance;
import Poly3D;

[vk::binding(0, 0)] ConstantBuffer<CameraInstance> gCamera;
[vk::binding(2, 0)] StructuredBuffer<PolyInstance3D> gMeshes;

struct VSOut
{
    float4 color : COLOR;
    float4 position : SV_Position;
}

[shader("vertex")]
VSOut main(
    [vk::location(0)] float3 inPosition : POSITION,
    [vk::location(1)] float4 inColor : COLOR,
    uint instanceId : SV_InstanceID
)
{
    PolyInstance3D mesh = gMeshes[instanceId];

    VSOut output;
            output.color    = inColor;
            output.position = mul(gCamera.projection, mul(gCamera.view, mul(mesh.model, float4(inPosition, 1.0))));
            output.position.z -= max(output.position.w, 0.0) * 0.01;

    return output;
}
