import CameraInstance;
import Point;

[vk::binding(0, 0)] ConstantBuffer<CameraInstance> gCamera;

struct VSOut
{
    float3 nearPoint : TEXCOORD0;
    float3 farPoint : TEXCOORD1;
    float4 position : SV_Position;
}

static const float3 positions[4] = {
    float3(-1.0, -1.0, 0.0),
    float3(1.0, -1.0, 0.0),
    float3(1.0, 1.0, 0.0),
    float3(-1.0, 1.0, 0.0)
};

#ifdef CHICANE_VULKAN
static const int indices[6] = {0, 2, 1, 2, 0, 3};
#else
static const int indices[6] = {0, 1, 2, 2, 3, 0};
#endif

[shader("vertex")]
VSOut main(uint vertexId : SV_VertexID)
{
    float3 point = positions[indices[vertexId]];

    VSOut output;
    output.nearPoint = UnprojectPoint(float3(point.x, point.y, 0.0), gCamera.projection, gCamera.view);
    output.farPoint  = UnprojectPoint(float3(point.x, point.y, 1.0), gCamera.projection, gCamera.view);
    output.position  = float4(point, 1.0);

    return output;
}
