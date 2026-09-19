module Point;

import Constant;

public float3 UnprojectPoint(float3 inPoint, float4x4 inProjection, float4x4 inView)
{
    float4 worldPoint = mul(inverse4x4(mul(inProjection, inView)), float4(inPoint, 1.0));

    return worldPoint.xyz / worldPoint.w;
}
