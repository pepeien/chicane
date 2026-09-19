module Target;

public uint meshInstanceIndex(uint instanceId)
{
    return instanceId;
}

public float2 uiVertexMultiplier()
{
#ifdef CHICANE_VULKAN
    return float2(1.0, -1.0);
#else
    return float2(1.0, 1.0);
#endif
}

public float2 uiScreenMultiplier()
{
#ifdef CHICANE_VULKAN
    return float2(1.0, 1.0);
#else
    return float2(1.0, -1.0);
#endif
}

public float2 uiClipPosition(float2 fragCoord, float2 view)
{
#ifdef CHICANE_VULKAN
    return fragCoord;
#else
    return float2(fragCoord.x, view.y - fragCoord.y);
#endif
}
