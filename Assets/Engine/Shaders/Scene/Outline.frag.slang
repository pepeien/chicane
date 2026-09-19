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

[shader("fragment")]
float4 main() : SV_Target
{
    return push.color;
}
