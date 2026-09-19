module Grid;

public static const float3 GRID_MINOR_COLOR = float3(0.3294, 0.3294, 0.3294);
public static const float3 GRID_MAJOR_COLOR = float3(0.45, 0.45, 0.45);
public static const float3 AXIS_X_COLOR     = float3(1.000, 0.200, 0.322);
public static const float3 AXIS_Y_COLOR     = float3(0.545, 0.863, 0.000);

public static const float GRID_CELL_MINOR  = 0.5;
public static const float GRID_CELL_MAJOR  = 5.0;
public static const float GRID_LINE_WIDTH  = 0.5;
public static const float MAJOR_LINE_WIDTH = 1.0;
public static const float AXIS_LINE_WIDTH  = 1.0;
public static const float MINOR_ALPHA      = 0.75;
public static const float MAJOR_ALPHA      = 1.0;

public float PixelCoverage(float inDistance, float inHalfWidth)
{
    return saturate(inHalfWidth + 0.5 - inDistance);
}

public float GridLines(float2 inPosition, float inCellSize, float inHalfWidth)
{
    float2 worldDerivative = max(fwidth(inPosition), float2(1e-6, 1e-6));
    float2 cellDistance    = abs(frac(inPosition / inCellSize - 0.5) - 0.5) * inCellSize;
    float2 pixelDistance   = cellDistance / worldDerivative;

    return PixelCoverage(min(pixelDistance.x, pixelDistance.y), inHalfWidth);
}

public float ComputeGridLinearDepth(float4 inClipSpacePosition, float inNear, float inFar)
{
    float depth = saturate(inClipSpacePosition.z / max(inClipSpacePosition.w, 1e-6));

    return (inNear * inFar) / max(inFar - depth * (inFar - inNear), 1e-6) / inFar;
}

public float ComputeGridFade(float3 inPosition, float3 inViewLocation, float inLinearDepth)
{
    float3 viewDir    = normalize(inPosition - inViewLocation);
    float3 gridNormal = float3(0.0, 0.0, 1.0);

    float angle     = abs(dot(viewDir, gridNormal));
    float angleFade = 1.0 - pow(1.0 - angle, 3.0);

    float height     = abs(inViewLocation.z);
    float heightFade = smoothstep(0.0, 10.0, height);

    float distance     = inLinearDepth;
    float distanceFade = pow(smoothstep(0.15, 0.85, distance), 1.5);

    return (1.0 - distanceFade) * angleFade * (1.0 - heightFade * 0.5);
}

public float ComputeGridDepth(float4 inWorldPosition)
{
    return (inWorldPosition.z / inWorldPosition.w) - 0.00001;
}

public float4 GridLayer(float3 inPosition, float3 inViewLocation, float inLinearDepth)
{
    float2 plane = inPosition.xy;

    float minor = GridLines(plane, GRID_CELL_MINOR, GRID_LINE_WIDTH);
    float major = GridLines(plane, GRID_CELL_MAJOR, MAJOR_LINE_WIDTH);

    float4 color = float4(GRID_MINOR_COLOR, minor * MINOR_ALPHA);
    color.rgb    = lerp(color.rgb, GRID_MAJOR_COLOR, major);
    color.a      = max(color.a, major * MAJOR_ALPHA);

    float2 axisDerivative = max(fwidth(plane), float2(1e-6, 1e-6));
    float xAxis           = PixelCoverage(abs(plane.y) / axisDerivative.y, AXIS_LINE_WIDTH);
    float yAxis           = PixelCoverage(abs(plane.x) / axisDerivative.x, AXIS_LINE_WIDTH);

    color.rgb = lerp(color.rgb, AXIS_X_COLOR, xAxis);
    color.rgb = lerp(color.rgb, AXIS_Y_COLOR, yAxis);
    color.a   = max(color.a, max(xAxis, yAxis));
    color.a *= ComputeGridFade(inPosition, inViewLocation, inLinearDepth);

    return color;
}
