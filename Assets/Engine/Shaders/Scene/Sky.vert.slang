import Target;
import CameraInstance;

[vk::binding(0, 0)] ConstantBuffer<CameraInstance> gCamera;

struct VSOut
{
    float3 uv : TEXCOORD0;
    float4 position : SV_Position;
}

[shader("vertex")]
VSOut main([vk::location(0)] float3 inPosition : POSITION)
{
    VSOut output;
    output.uv = inPosition;
    float3 viewDir    = mul((float3x3)gCamera.view, inPosition);
    output.position   = mul(gCamera.projection, float4(viewDir, 0.0));
    output.position.z = output.position.w;

    return output;
}
