import CameraInstance;
import Grid;

[vk::binding(0, 0)] ConstantBuffer<CameraInstance> gCamera;

struct GridPush
{
    float4 color;
    float4 cells;
    float4 extra;
}

#ifdef CHICANE_VULKAN
[vk::push_constant]
#else
[vk::binding(15, 0)]
#endif
ConstantBuffer<GridPush> push;

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
    float depth = 1.0;
    float4 color = EvaluateGrid(
        inData.nearPoint,
        inData.farPoint,
        gCamera.translation.xyz,
        gCamera.projection,
        gCamera.view,
        gCamera.clip.xy,
        push.color,
        push.cells.xy,
        float3(push.cells.z, push.cells.w, push.extra.x),
        depth
    );

    if (color.a <= 0.0)
    {
        discard;
    }

    FSOut output;
    output.color = color;
    output.depth = depth;

    return output;
}
