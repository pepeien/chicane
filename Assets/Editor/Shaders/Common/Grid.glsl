const vec3 GRID_MINOR_COLOR = vec3(0.3294);
const vec3 GRID_MAJOR_COLOR = vec3(0.45);
const vec3 AXIS_X_COLOR     = vec3(1.000, 0.200, 0.322);
const vec3 AXIS_Y_COLOR     = vec3(0.545, 0.863, 0.000);

const float GRID_CELL_MINOR  = 0.5;
const float GRID_CELL_MAJOR  = 5.0;
const float GRID_LINE_WIDTH  = 0.5;
const float MAJOR_LINE_WIDTH = 1.0;
const float AXIS_LINE_WIDTH  = 1.0;
const float MINOR_ALPHA      = 0.75;
const float MAJOR_ALPHA      = 1.0;

float PixelCoverage(float inDistance, float inHalfWidth) {
    return clamp(inHalfWidth + 0.5 - inDistance, 0.0, 1.0);
}

float GridLines(vec2 inPosition, float inCellSize, float inHalfWidth) {
    vec2 worldDerivative = max(fwidth(inPosition), vec2(1e-6));
    vec2 cellDistance    = abs(fract(inPosition / inCellSize - 0.5) - 0.5) * inCellSize;
    vec2 pixelDistance   = cellDistance / worldDerivative;

    return PixelCoverage(min(pixelDistance.x, pixelDistance.y), inHalfWidth);
}

float ComputeGridLinearDepth(vec4 inClipSpacePosition, float inNear, float inFar) {
    float depth = inClipSpacePosition.z / inClipSpacePosition.w;

    return ((2.0 * inNear * inFar) / (inFar + inNear - depth * (inFar - inNear))) / inFar;
}

float ComputeGridFade(vec3 inPosition, vec3 inViewLocation, float inLinearDepth) {
    vec3 viewDir    = normalize(inPosition - inViewLocation);
    vec3 gridNormal = vec3(0.0, 0.0, 1.0);

    float angle     = abs(dot(viewDir, gridNormal));
    float angleFade = 1.0 - pow(1.0 - angle, 3.0);

    float height     = abs(inViewLocation.z);
    float heightFade = smoothstep(0.0, 10.0, height);

    float distance     = inLinearDepth;
    float distanceFade = pow(smoothstep(0.15, 0.85, distance), 1.5);

    return (1.0 - distanceFade) * angleFade * (1.0 - heightFade * 0.5);
}

float ComputeGridDepth(vec4 inWorldPosition) {
    return (inWorldPosition.z / inWorldPosition.w) - 0.00001;
}

vec4 GridLayer(vec3 inPosition, vec3 inViewLocation, float inLinearDepth) {
    vec2 plane = inPosition.xy;

    float minor = GridLines(plane, GRID_CELL_MINOR, GRID_LINE_WIDTH);
    float major = GridLines(plane, GRID_CELL_MAJOR, MAJOR_LINE_WIDTH);

    vec4 color = vec4(GRID_MINOR_COLOR, minor * MINOR_ALPHA);
    color.rgb  = mix(color.rgb, GRID_MAJOR_COLOR, major);
    color.a    = max(color.a, major * MAJOR_ALPHA);

    vec2 axisDerivative = max(fwidth(plane), vec2(1e-6));
    float xAxis         = PixelCoverage(abs(plane.y) / axisDerivative.y, AXIS_LINE_WIDTH);
    float yAxis         = PixelCoverage(abs(plane.x) / axisDerivative.x, AXIS_LINE_WIDTH);

    color.rgb = mix(color.rgb, AXIS_X_COLOR, xAxis);
    color.rgb = mix(color.rgb, AXIS_Y_COLOR, yAxis);
    color.a   = max(color.a, max(xAxis, yAxis));
    color.a  *= ComputeGridFade(inPosition, inViewLocation, inLinearDepth);

    return color;
}
