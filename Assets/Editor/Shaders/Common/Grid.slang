module Grid;

public static const float3 GRID_MINOR_COLOR = float3(0.3294, 0.3294, 0.3294);
public static const float3 GRID_MAJOR_COLOR = float3(0.45, 0.45, 0.45);
public static const float3 AXIS_X_COLOR     = float3(1.000, 0.200, 0.322);
public static const float3 AXIS_Y_COLOR     = float3(0.545, 0.863, 0.000);
public static const float3 AXIS_Z_COLOR     = float3(0.000, 0.533, 0.862);

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

public float ComputeAxisFade(float inLinearDepth)
{
    return 1.0 - pow(smoothstep(0.15, 0.85, inLinearDepth), 1.5);
}

public float ComputeGridDepth(float4 inWorldPosition)
{
    return (inWorldPosition.z / inWorldPosition.w) - 0.00001;
}

public float4 FloorGrid(float3 inPosition, float3 inViewLocation, float inLinearDepth, float4 inColor, float2 inCells, float2 inAxes)
{
    float2 plane = inPosition.xy;

    float minorSize = max(inCells.x, 1e-4);
    float majorSize = max(inCells.y, minorSize);

    float minor = GridLines(plane, minorSize, GRID_LINE_WIDTH);
    float major = GridLines(plane, majorSize, MAJOR_LINE_WIDTH);

    float3 majorColor = saturate(inColor.rgb + float3(0.12, 0.12, 0.12));
    float4 color      = float4(inColor.rgb, minor * MINOR_ALPHA);
    color.rgb         = lerp(color.rgb, majorColor, major);
    color.a           = max(color.a, major * MAJOR_ALPHA);

    float2 axisDerivative = max(fwidth(plane), float2(1e-6, 1e-6));
    float xAxis           = PixelCoverage(abs(plane.y) / axisDerivative.y, AXIS_LINE_WIDTH) * inAxes.x;
    float yAxis           = PixelCoverage(abs(plane.x) / axisDerivative.x, AXIS_LINE_WIDTH) * inAxes.y;

    color.rgb = lerp(color.rgb, AXIS_X_COLOR, xAxis);
    color.rgb = lerp(color.rgb, AXIS_Y_COLOR, yAxis);
    color.a   = max(color.a, max(xAxis, yAxis));
    color.a *= ComputeGridFade(inPosition, inViewLocation, inLinearDepth);

    return color;
}

public void PlaneHit(float3 inNearPoint, float3 inFarPoint, float3 inNormal, out float3 outPosition, out float outT)
{
    float3 ray   = inFarPoint - inNearPoint;
    float  denom = dot(ray, inNormal);
    outT         = -dot(inNearPoint, inNormal) / (abs(denom) < 1e-8 ? 1e-8 : denom);
    outPosition  = inNearPoint + outT * ray;
}

public float4 ZAxis(float3 inClosest, float inT, float inCoverage, float inLinearDepth)
{
    float dist      = length(inClosest.xy);
    float pixelDist = dist / max(fwidth(dist), 1e-6);
    float fade      = ComputeAxisFade(inLinearDepth) * inCoverage * (inT >= 0.0 ? 1.0 : 0.0);

    return float4(AXIS_Z_COLOR, PixelCoverage(pixelDist, AXIS_LINE_WIDTH) * fade);
}

public void Consider(inout float4 ioColor, inout float ioDepth, float4 inColor, float inDepth)
{
    if (inColor.a > ioColor.a)
    {
        ioColor = inColor;
        ioDepth = inDepth;
    }
}

public float4 EvaluateGrid(
    float3   inNearPoint,
    float3   inFarPoint,
    float3   inViewLocation,
    float4x4 inProjection,
    float4x4 inView,
    float2   inClip,
    float4   inColor,
    float2   inCells,
    float3   inAxes,
    out float outDepth
)
{
    float4 color = float4(0.0, 0.0, 0.0, 0.0);
    outDepth     = 1.0;

    float3 position;
    float  t;

    PlaneHit(inNearPoint, inFarPoint, float3(0.0, 0.0, 1.0), position, t);
    {
        float4 worldPosition = mul(inProjection, mul(inView, float4(position, 1.0)));
        float  linearDepth   = ComputeGridLinearDepth(worldPosition, inClip.x, inClip.y);
        float4 floorColor    = FloorGrid(position, inViewLocation, linearDepth, inColor, inCells, inAxes.xy);
        floorColor.a *= t >= 0.0 ? 1.0 : 0.0;
        Consider(color, outDepth, floorColor, ComputeGridDepth(worldPosition));
    }

    {
        float3 ray     = inFarPoint - inNearPoint;
        float2 dir     = ray.xy;
        float  tZ      = -dot(inNearPoint.xy, dir) / max(dot(dir, dir), 1e-8);
        float3 closest = inNearPoint + tZ * ray;

        float4 worldPosition = mul(inProjection, mul(inView, float4(closest, 1.0)));
        float  linearDepth   = ComputeGridLinearDepth(worldPosition, inClip.x, inClip.y);
        Consider(
            color,
            outDepth,
            ZAxis(closest, tZ, inAxes.z, linearDepth),
            ComputeGridDepth(worldPosition)
        );
    }

    return color;
}
