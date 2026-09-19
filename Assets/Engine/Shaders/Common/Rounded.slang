module Rounded;

public bool hasRoundedCorners(float4 inRadiusX, float4 inRadiusY)
{
    return max(max(inRadiusX.x, inRadiusX.y), max(inRadiusX.z, inRadiusX.w)) > 0.0 ||
           max(max(inRadiusY.x, inRadiusY.y), max(inRadiusY.z, inRadiusY.w)) > 0.0;
}

public float roundedRectSDF(float2 inPoint, float4 inBox, float4 inRadiusX, float4 inRadiusY)
{
    float2 size     = max(inBox.zw - inBox.xy, float2(1e-4, 1e-4));
    float2 p        = inPoint - lerp(inBox.xy, inBox.zw, 0.5);
    float2 halfSize = size * 0.5;
    float2 radius   = float2(0.0, 0.0);

    if (p.x < 0.0)
    {
        radius = (p.y < 0.0) ? float2(inRadiusX.x, inRadiusY.x) : float2(inRadiusX.w, inRadiusY.w);
    }
    else
    {
        radius = (p.y < 0.0) ? float2(inRadiusX.y, inRadiusY.y) : float2(inRadiusX.z, inRadiusY.z);
    }

    float r = min(min(max(radius.x, 0.0), max(radius.y, 0.0)), min(halfSize.x, halfSize.y));
    float2 q = abs(p) - halfSize + r;

    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

public float sdCoverage(float inDistance)
{
    return saturate(0.5 - inDistance / max(fwidth(inDistance), 1e-4));
}

public bool insideRoundedRect(float2 inPoint, float4 inBox, float4 inRadiusX, float4 inRadiusY)
{
    float distance = roundedRectSDF(inPoint, inBox, inRadiusX, inRadiusY);

    return distance <= max(0.5 * fwidth(distance), 0.5);
}

public float roundedRectCoverage(float2 inPoint, float4 inBox, float4 inRadiusX, float4 inRadiusY)
{
    return sdCoverage(roundedRectSDF(inPoint, inBox, inRadiusX, inRadiusY));
}

public float4 pickBorderColor(
    float2 inPoint,
    float4 inBox,
    float4 inWidth,
    float4 inColorTop,
    float4 inColorRight,
    float4 inColorBottom,
    float4 inColorLeft
)
{
    float2 size    = max(inBox.zw - inBox.xy, float2(1e-4, 1e-4));
    float2 local   = inPoint - inBox.xy;
    float nearest  = 1e9;
    float4 color   = inColorBottom;

    if (inWidth.x > 0.0)
    {
        nearest = local.y;
        color   = inColorTop;
    }

    if (inWidth.y > 0.0 && (size.x - local.x) < nearest)
    {
        nearest = size.x - local.x;
        color   = inColorRight;
    }

    if (inWidth.z > 0.0 && (size.y - local.y) < nearest)
    {
        nearest = size.y - local.y;
        color   = inColorBottom;
    }

    if (inWidth.w > 0.0 && local.x < nearest)
    {
        color = inColorLeft;
    }

    return color;
}

public float4 applyRoundedBorder(
    float4 inFill,
    float2 inPoint,
    float4 inBox,
    float4 inRadiusX,
    float4 inRadiusY,
    float4 inWidth,
    float4 inColorTop,
    float4 inColorRight,
    float4 inColorBottom,
    float4 inColorLeft
)
{
    float maxWidth = max(max(inWidth.x, inWidth.y), max(inWidth.z, inWidth.w));

    if (maxWidth <= 0.0)
    {
        return inFill;
    }

    float4 innerBox = float4(inBox.x + inWidth.w, inBox.y + inWidth.x, inBox.z - inWidth.y, inBox.w - inWidth.z);
    innerBox.z      = max(innerBox.x, innerBox.z);
    innerBox.w      = max(innerBox.y, innerBox.w);

    float4 innerRadiusX = max(inRadiusX - float4(inWidth.w, inWidth.y, inWidth.y, inWidth.w), float4(0.0, 0.0, 0.0, 0.0));
    float4 innerRadiusY = max(inRadiusY - float4(inWidth.x, inWidth.x, inWidth.z, inWidth.z), float4(0.0, 0.0, 0.0, 0.0));

    float outerDistance = roundedRectSDF(inPoint, inBox, inRadiusX, inRadiusY);
    float innerDistance = roundedRectSDF(inPoint, innerBox, innerRadiusX, innerRadiusY);
    float outerCoverage = sdCoverage(outerDistance);
    float innerCoverage = sdCoverage(innerDistance);
    float ring          = saturate(outerCoverage - innerCoverage);

    if (ring <= 0.0)
    {
        return inFill;
    }

    float4 borderColor = pickBorderColor(
        inPoint,
        inBox,
        inWidth,
        inColorTop,
        inColorRight,
        inColorBottom,
        inColorLeft
    );

    return lerp(inFill, borderColor, ring / max(outerCoverage, 1e-4));
}
