module Triplanar;

public float3 triplanarBlend(float3 geometricNormal)
{
    float3 blend = pow(abs(normalize(geometricNormal)), 4.0);
    float sum    = blend.x + blend.y + blend.z;

    return blend / max(sum, 1e-5);
}

public float2 triplanarUvX(float3 worldPos, float tileSize)
{
    return worldPos.zy / tileSize;
}

public float2 triplanarUvY(float3 worldPos, float tileSize)
{
    return worldPos.xz / tileSize;
}

public float2 triplanarUvZ(float3 worldPos, float tileSize)
{
    return worldPos.xy / tileSize;
}
