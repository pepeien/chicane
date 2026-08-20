float filterGaussianCoverage(float inSignedDistance, float inSigma) {
    float width = max(inSigma * 1.41421356, max(fwidth(inSignedDistance), 0.75));

    return clamp(0.5 - (0.5 * inSignedDistance / width), 0.0, 1.0);
}

vec3 sampleBackdropBlur(sampler2D inBackdrop, vec2 inFragCoord, vec2 inView, float inSigma) {
    vec2 uv = inFragCoord / max(inView, vec2(1.0));
    uv      = clamp(uv, vec2(0.0), vec2(1.0));

    float sigma = max(inSigma, 0.0);

    if (sigma < 0.5) {
        return texture(inBackdrop, uv).rgb;
    }

    vec2  size0    = vec2(textureSize(inBackdrop, 0));
    float lod      = clamp(log2(max(sigma * 0.5, 1.0)), 0.0, 8.0);
    float texelPx  = exp2(lod);
    vec2  stepUv   = vec2(texelPx) / max(size0, vec2(1.0));
    float sigmaTex = sigma / max(texelPx, 1.0);
    float twoVar   = 2.0 * sigmaTex * sigmaTex;
    vec3  acc      = vec3(0.0);
    float wsum     = 0.0;

    for (int y = -4; y <= 4; y++) {
        for (int x = -4; x <= 4; x++) {
            float px  = float(x);
            float py  = float(y);
            float w   = exp(-(px * px + py * py) / twoVar);
            vec2  tap = clamp(uv + (vec2(px, py) * stepUv), 0.0, 1.0);

            acc  += textureLod(inBackdrop, tap, lod).rgb * w;
            wsum += w;
        }
    }

    return acc / max(wsum, 1e-4);
}
