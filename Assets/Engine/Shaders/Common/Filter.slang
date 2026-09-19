module Filter;

public float filterGaussianCoverage(float inSignedDistance, float inSigma)
{
    float width = max(inSigma * 1.41421356, max(fwidth(inSignedDistance), 0.75));

    return saturate(0.5 - (0.5 * inSignedDistance / width));
}

public float3 sampleBackdropBlur(Sampler2D inBackdrop, float2 inFragCoord, float2 inView, float inSigma)
{
    float2 uv = inFragCoord / max(inView, float2(1.0, 1.0));
    uv        = clamp(uv, float2(0.0, 0.0), float2(1.0, 1.0));

    float sigma = max(inSigma, 0.0);

    if (sigma < 0.5)
    {
        return inBackdrop.Sample(uv).rgb;
    }

    uint width = 1;
    uint height = 1;
    inBackdrop.GetDimensions(width, height);
    float2 size0    = float2(width, height);
    float lod       = clamp(log2(max(sigma * 0.5, 1.0)), 0.0, 8.0);
    float texelPx   = exp2(lod);
    float2 stepUv   = float2(texelPx, texelPx) / max(size0, float2(1.0, 1.0));
    float sigmaTex  = sigma / max(texelPx, 1.0);
    float twoVar    = 2.0 * sigmaTex * sigmaTex;
    float3 acc      = float3(0.0, 0.0, 0.0);
    float wsum      = 0.0;

    for (int y = -4; y <= 4; y++)
    {
        for (int x = -4; x <= 4; x++)
        {
            float px  = float(x);
            float py  = float(y);
            float w   = exp(-(px * px + py * py) / twoVar);
            float2 tap = clamp(uv + (float2(px, py) * stepUv), 0.0, 1.0);

            acc  += inBackdrop.SampleLevel(tap, lod).rgb * w;
            wsum += w;
        }
    }

    return acc / max(wsum, 1e-4);
}
