module Constant;

public static const float4x4 SHADOW_BIAS = float4x4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0
);

public float2x2 inverse2x2(float2x2 m)
{
    float2 c0  = mul(m, float2(1.0, 0.0));
    float2 c1  = mul(m, float2(0.0, 1.0));
    float  det = (c0.x * c1.y) - (c0.y * c1.x);
    return float2x2(c1.y, -c1.x, -c0.y, c0.x) / det;
}

public float3x3 inverse3x3(float3x3 m)
{
    float3 c0 = mul(m, float3(1.0, 0.0, 0.0));
    float3 c1 = mul(m, float3(0.0, 1.0, 0.0));
    float3 c2 = mul(m, float3(0.0, 0.0, 1.0));
    float3 r0 = cross(c1, c2);
    float3 r1 = cross(c2, c0);
    float3 r2 = cross(c0, c1);
    return float3x3(r0, r1, r2) / dot(c0, r0);
}

public float4x4 inverse4x4(float4x4 m)
{
    float4 c0 = mul(m, float4(1.0, 0.0, 0.0, 0.0));
    float4 c1 = mul(m, float4(0.0, 1.0, 0.0, 0.0));
    float4 c2 = mul(m, float4(0.0, 0.0, 1.0, 0.0));
    float4 c3 = mul(m, float4(0.0, 0.0, 0.0, 1.0));

    float a00 = c0.x, a10 = c0.y, a20 = c0.z, a30 = c0.w;
    float a01 = c1.x, a11 = c1.y, a21 = c1.z, a31 = c1.w;
    float a02 = c2.x, a12 = c2.y, a22 = c2.z, a32 = c2.w;
    float a03 = c3.x, a13 = c3.y, a23 = c3.z, a33 = c3.w;

    float b00 = (a00 * a11) - (a01 * a10);
    float b01 = (a00 * a12) - (a02 * a10);
    float b02 = (a00 * a13) - (a03 * a10);
    float b03 = (a01 * a12) - (a02 * a11);
    float b04 = (a01 * a13) - (a03 * a11);
    float b05 = (a02 * a13) - (a03 * a12);
    float b06 = (a20 * a31) - (a21 * a30);
    float b07 = (a20 * a32) - (a22 * a30);
    float b08 = (a20 * a33) - (a23 * a30);
    float b09 = (a21 * a32) - (a22 * a31);
    float b10 = (a21 * a33) - (a23 * a31);
    float b11 = (a22 * a33) - (a23 * a32);

    float det    = (b00 * b11) - (b01 * b10) + (b02 * b09) + (b03 * b08) - (b04 * b07) + (b05 * b06);
    float invDet = 1.0 / det;

    return float4x4(
        float4(
            ((a11 * b11) - (a12 * b10) + (a13 * b09)) * invDet,
            ((a02 * b10) - (a01 * b11) - (a03 * b09)) * invDet,
            ((a31 * b05) - (a32 * b04) + (a33 * b03)) * invDet,
            ((a22 * b04) - (a21 * b05) - (a23 * b03)) * invDet
        ),
        float4(
            ((a12 * b08) - (a10 * b11) - (a13 * b07)) * invDet,
            ((a00 * b11) - (a02 * b08) + (a03 * b07)) * invDet,
            ((a32 * b02) - (a30 * b05) - (a33 * b01)) * invDet,
            ((a20 * b05) - (a22 * b02) + (a23 * b01)) * invDet
        ),
        float4(
            ((a10 * b10) - (a11 * b08) + (a13 * b06)) * invDet,
            ((a01 * b08) - (a00 * b10) - (a03 * b06)) * invDet,
            ((a30 * b04) - (a31 * b02) + (a33 * b00)) * invDet,
            ((a21 * b02) - (a20 * b04) - (a23 * b00)) * invDet
        ),
        float4(
            ((a11 * b07) - (a10 * b09) - (a12 * b06)) * invDet,
            ((a00 * b09) - (a01 * b07) + (a02 * b06)) * invDet,
            ((a31 * b01) - (a30 * b03) - (a32 * b00)) * invDet,
            ((a20 * b03) - (a21 * b01) + (a22 * b00)) * invDet
        )
    );
}
