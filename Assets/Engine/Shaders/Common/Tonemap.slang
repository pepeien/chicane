module Tonemap;

public float3 acesTonemap(float3 color)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return saturate((color * (a * color + b)) / (color * (c * color + d) + e));
}

public float luminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}
