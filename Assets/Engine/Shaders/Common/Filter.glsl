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

    const int radius = 4;
    float     span   = sigma * 2.0;
    float     stepPx = span / float(radius);
    vec2      stepUv = vec2(stepPx) / max(inView, vec2(1.0));
    float     twoVar = 2.0 * sigma * sigma;
    vec3      acc    = vec3(0.0);
    float     wsum   = 0.0;

    for (int y = -4; y <= 4; y++) {
        for (int x = -4; x <= 4; x++) {
            float px  = float(x) * stepPx;
            float py  = float(y) * stepPx;
            float w   = exp(-(px * px + py * py) / twoVar);
            vec2  tap = clamp(uv + vec2(float(x), float(y)) * stepUv, 0.0, 1.0);

            acc  += texture(inBackdrop, tap).rgb * w;
            wsum += w;
        }
    }

    return acc / max(wsum, 1e-4);
}
