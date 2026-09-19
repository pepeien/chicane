import Constant;
import Target;
import CameraInstance;
import Poly3D;

[vk::binding(0, 0)] ConstantBuffer<CameraInstance> gCamera;
[vk::binding(2, 0)] StructuredBuffer<PolyInstance3D> gMeshes;

struct VSOut
{
    nointerpolation int4 textureMaps0 : TEXCOORD0;
    nointerpolation int4 textureMaps1 : TEXCOORD1;
    nointerpolation int4 textureMaps2 : TEXCOORD2;
    nointerpolation int bIsLit : TEXCOORD3;
    nointerpolation float emissiveStrength : TEXCOORD4;
    nointerpolation float tileSize : TEXCOORD5;
    float2 uv : TEXCOORD6;
    float3 normal : TEXCOORD7;
    float4 tangent : TEXCOORD8;
    float4 color : COLOR;
    float3 meshPosition : TEXCOORD9;
    float3 cameraPosition : TEXCOORD10;
    float viewDepth : TEXCOORD11;
    float4 position : SV_Position;
}

[shader("vertex")]
VSOut main(
    [vk::location(0)] float3 inPosition : POSITION,
    [vk::location(1)] float4 inColor : COLOR,
    [vk::location(2)] float2 inUV : TEXCOORD0,
    [vk::location(3)] float3 inNormal : NORMAL,
    [vk::location(4)] float4 inTangent : TANGENT,
    uint instanceId : SV_InstanceID
)
{
    PolyInstance3D mesh = gMeshes[instanceId];

    float4 position       = float4(inPosition, 1.0);
    float3x3 normalMatrix = transpose(inverse3x3((float3x3)mesh.model));

    VSOut output;
    output.textureMaps0     = mesh.textures0;
    output.textureMaps1     = mesh.textures1;
    output.textureMaps2     = mesh.textures2;
    output.bIsLit           = (mesh.flags & POLY_FLAG_LIT) != 0 ? 1 : 0;
    output.emissiveStrength = mesh.emissiveStrength;
    output.tileSize         = mesh.tileSize;
    output.uv               = inUV;
    output.normal           = mul(normalMatrix, inNormal);
    output.tangent          = float4(mul(normalMatrix, inTangent.xyz), inTangent.w);
    output.color            = inColor;
    output.meshPosition   = mul(mesh.model, position).xyz;
    float4 viewPosition   = mul(gCamera.view, float4(output.meshPosition, 1.0));
    float4 cameraOrigin   = mul(inverse4x4(gCamera.view), float4(0.0, 0.0, 0.0, 1.0));
    output.cameraPosition = cameraOrigin.xyz / max(cameraOrigin.w, 1e-6);
    output.viewDepth      = -viewPosition.z;
    output.position       = mul(gCamera.projection, viewPosition);

    return output;
}
