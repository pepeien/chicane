vec4 GridCalculation(vec3 inPosition, float inScale, bool inHasAxis) {
    vec2 coordinates = inPosition.xy * inScale;
    vec2 derivative  = fwidth(coordinates);
    vec2 uv          = fract(coordinates - 0.5) - 0.5;
    vec2 grid        = abs(uv) / derivative;

    float line     = min(grid.x, grid.y);
    float minimumx = min(derivative.x, 1.0);
    float minimumy = min(derivative.y, 1.0);

    vec4 color = vec4(0.14);
    color.a    = 1.0 - min(line, 1.0);

    if (!inHasAxis) {
        return color;
    }

    if (inPosition.x > (-0.1 * minimumx) && inPosition.x < (0.1 * minimumx)) {
        color.rgb = vec3(0.94, 0.15, 0.22);
    }

    if (inPosition.y > (-0.1 * minimumy) && inPosition.y < (0.1 * minimumy)) {
        color.rgb = vec3(0.21, 0.21, 0.56);
    }

    return color;
}

float ComputeGridLinearDepth(vec4 inClipSpacePosition, float inNear, float inFar) {
    float depth = inClipSpacePosition.z / inClipSpacePosition.w;

    return ((2.0 * inNear * inFar) / (inFar + inNear - depth * (inFar - inNear))) / inFar;
}

float ComputeGridFade(vec3 inPosition, vec3 inViewLocation, float inLinearDepth) {
    vec3 viewDir = normalize(inPosition - inViewLocation);
    vec3 gridNormal = vec3(0.0, 0.0, 1.0);

    float angle     = abs(dot(viewDir, gridNormal));
    float angleFade = smoothstep(0.05, 0.25, angle);

    float height     = abs(inViewLocation.z);
    float heightFade = smoothstep(0.0, 10.0, height);

    float distance     = inLinearDepth;
    float distanceFade = pow(smoothstep(0.15, 0.85, distance), 1.5);

    return (1.0 - distanceFade) * angleFade * (1.0 - heightFade * 0.5);
}

float ComputeGridDepth(vec4 inWorldPosition) {
    return (inWorldPosition.z / inWorldPosition.w) - 0.00001;
}