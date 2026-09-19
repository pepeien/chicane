module Poly2D;

public struct PolyInstance2D
{
    public float2 view;
    public float2 scale;
    public float2 size;
    public float2 offset;
    public float3 position;
    public float backdropBlur;
    public float4 color;
    public float4 clip;
    public int textureIndex;
    public int glyphIndex;
    public float glyphDilation;
    public float filterBlur;
    public float2 transformX;
    public float2 transformY;
    public float4 radiusX;
    public float4 radiusY;
    public float4 innerClip;
    public float4 innerClipRadiusX;
    public float4 innerClipRadiusY;
    public float4 outerClip;
    public float4 outerClipRadiusX;
    public float4 outerClipRadiusY;
    public float4 borderWidth;
    public float4 borderColorTop;
    public float4 borderColorRight;
    public float4 borderColorBottom;
    public float4 borderColorLeft;
    public int gradientType;
    public int gradientStopCount;
    public float2 gradientAxis;
    public float4 gradientStops[8];
    public float4 gradientOffsets0;
    public float4 gradientOffsets1;
}

public float normalize2DDepth(float inValue)
{
    float result = inValue;
    result /= 999.0;
    result = saturate(abs(result - 1.0));

    return result;
}

public float2 get2DVertexPosition(PolyInstance2D inInstance, float2 inPosition, float2 inMultipliers)
{
    float2 position = inPosition;
    position *= inInstance.scale;
    position += inInstance.offset;
    position  = (inInstance.transformX * position.x) + (inInstance.transformY * position.y);
    position /= inInstance.view;
    position *= 2.0;
    position *= inMultipliers;

    return position;
}

public float3 get2DScreenPosition(PolyInstance2D inInstance, float2 inMultipliers)
{
    float3 position = inInstance.position;
    position.xy += inInstance.size * 0.5;
    position.xy /= inInstance.view;
    position.xy  = (position.xy * 2.0) - 1.0;
    position.xy *= inMultipliers;
    position.z   = normalize2DDepth(position.z);

    return position;
}

public float4 normalize2DColor(float4 inColor)
{
    return inColor / 255.0;
}

public static const float FILTER_BLUR_SUPPORT = 3.0;

public float2 get2DBlurredScale(PolyInstance2D inInstance)
{
    return inInstance.scale + float2(inInstance.filterBlur * FILTER_BLUR_SUPPORT * 2.0, inInstance.filterBlur * FILTER_BLUR_SUPPORT * 2.0);
}

public float2 get2DFilterUV(float2 inUV, float2 inOriginalScale, float2 inBlurredScale)
{
    float2 uv = inUV;

    if (inOriginalScale.x > 1e-4)
    {
        uv.x = ((inUV.x - 0.5) * (inBlurredScale.x / inOriginalScale.x)) + 0.5;
    }

    if (inOriginalScale.y > 1e-4)
    {
        uv.y = ((inUV.y - 0.5) * (inBlurredScale.y / inOriginalScale.y)) + 0.5;
    }

    return uv;
}

public float4 samplePoly2DGradient(PolyInstance2D inInstance, float2 inUV)
{
    if (inInstance.gradientType <= 0 || inInstance.gradientStopCount <= 0)
    {
        return normalize2DColor(inInstance.color);
    }

    float t = 0.0;

    if (inInstance.gradientType == 1)
    {
        float2 axis  = inInstance.gradientAxis;
        float2 start = float2(0.5, 0.5) - (axis * 0.5);
        float denom  = max(dot(axis, axis), 1e-5);
        t            = dot(inUV - start, axis) / denom;
    }
    else
    {
        float2 center = inInstance.gradientAxis;
        float2 extent = max(center, float2(1.0, 1.0) - center);
        float radius  = max(length(extent), 1e-5);
        t             = distance(inUV, center) / radius;
    }

    t = saturate(t);

    int count = clamp(inInstance.gradientStopCount, 1, 8);
    float offsets[8];
    offsets[0] = inInstance.gradientOffsets0.x;
    offsets[1] = inInstance.gradientOffsets0.y;
    offsets[2] = inInstance.gradientOffsets0.z;
    offsets[3] = inInstance.gradientOffsets0.w;
    offsets[4] = inInstance.gradientOffsets1.x;
    offsets[5] = inInstance.gradientOffsets1.y;
    offsets[6] = inInstance.gradientOffsets1.z;
    offsets[7] = inInstance.gradientOffsets1.w;

    if (t <= offsets[0])
    {
        return normalize2DColor(inInstance.gradientStops[0]);
    }

    if (t >= offsets[count - 1])
    {
        return normalize2DColor(inInstance.gradientStops[count - 1]);
    }

    for (int i = 0; i < 7; i++)
    {
        if (i >= count - 1)
        {
            break;
        }

        float start = offsets[i];
        float end   = offsets[i + 1];

        if (t < start || t > end)
        {
            continue;
        }

        float span = max(end - start, 1e-5);
        float u    = (t - start) / span;

        return lerp(normalize2DColor(inInstance.gradientStops[i]), normalize2DColor(inInstance.gradientStops[i + 1]), u);
    }

    return normalize2DColor(inInstance.gradientStops[count - 1]);
}
