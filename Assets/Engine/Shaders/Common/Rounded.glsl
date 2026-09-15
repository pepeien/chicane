bool hasRoundedCorners(vec4 inRadiusX, vec4 inRadiusY) {
    return max(max(inRadiusX.x, inRadiusX.y), max(inRadiusX.z, inRadiusX.w)) > 0.0 ||
           max(max(inRadiusY.x, inRadiusY.y), max(inRadiusY.z, inRadiusY.w)) > 0.0;
}

float roundedRectSDF(vec2 inPoint, vec4 inBox, vec4 inRadiusX, vec4 inRadiusY) {
    vec2 size     = max(inBox.zw - inBox.xy, vec2(1e-4));
    vec2 p        = inPoint - mix(inBox.xy, inBox.zw, 0.5);
    vec2 halfSize = size * 0.5;
    vec2 radius   = vec2(0.0);

    if (p.x < 0.0) {
        radius = (p.y < 0.0) ? vec2(inRadiusX.x, inRadiusY.x) : vec2(inRadiusX.w, inRadiusY.w);
    } else {
        radius = (p.y < 0.0) ? vec2(inRadiusX.y, inRadiusY.y) : vec2(inRadiusX.z, inRadiusY.z);
    }

    float r = min(min(max(radius.x, 0.0), max(radius.y, 0.0)), min(halfSize.x, halfSize.y));
    vec2  q = abs(p) - halfSize + r;

    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

float sdCoverage(float inDistance) {
    return clamp(0.5 - inDistance / max(fwidth(inDistance), 1e-4), 0.0, 1.0);
}

bool insideRoundedRect(vec2 inPoint, vec4 inBox, vec4 inRadiusX, vec4 inRadiusY) {
    float distance = roundedRectSDF(inPoint, inBox, inRadiusX, inRadiusY);

    return distance <= max(0.5 * fwidth(distance), 0.5);
}

float roundedRectCoverage(vec2 inPoint, vec4 inBox, vec4 inRadiusX, vec4 inRadiusY) {
    return sdCoverage(roundedRectSDF(inPoint, inBox, inRadiusX, inRadiusY));
}

vec4 pickBorderColor(
    vec2 inPoint,
    vec4 inBox,
    vec4 inWidth,
    vec4 inColorTop,
    vec4 inColorRight,
    vec4 inColorBottom,
    vec4 inColorLeft
) {
    vec2 size = max(inBox.zw - inBox.xy, vec2(1e-4));
    vec2 local = inPoint - inBox.xy;
    float nearest = 1e9;
    vec4 color = inColorBottom;

    if (inWidth.x > 0.0) {
        nearest = local.y;
        color = inColorTop;
    }

    if (inWidth.y > 0.0 && (size.x - local.x) < nearest) {
        nearest = size.x - local.x;
        color = inColorRight;
    }

    if (inWidth.z > 0.0 && (size.y - local.y) < nearest) {
        nearest = size.y - local.y;
        color = inColorBottom;
    }

    if (inWidth.w > 0.0 && local.x < nearest) {
        color = inColorLeft;
    }

    return color;
}

vec4 applyRoundedBorder(
    vec4 inFill,
    vec2 inPoint,
    vec4 inBox,
    vec4 inRadiusX,
    vec4 inRadiusY,
    vec4 inWidth,
    vec4 inColorTop,
    vec4 inColorRight,
    vec4 inColorBottom,
    vec4 inColorLeft
) {
    float maxWidth = max(max(inWidth.x, inWidth.y), max(inWidth.z, inWidth.w));

    if (maxWidth <= 0.0) {
        return inFill;
    }

    vec4 innerBox = vec4(
        inBox.x + inWidth.w,
        inBox.y + inWidth.x,
        inBox.z - inWidth.y,
        inBox.w - inWidth.z
    );
    innerBox.z = max(innerBox.x, innerBox.z);
    innerBox.w = max(innerBox.y, innerBox.w);

    vec4 innerRadiusX = max(inRadiusX - vec4(inWidth.w, inWidth.y, inWidth.y, inWidth.w), vec4(0.0));
    vec4 innerRadiusY = max(inRadiusY - vec4(inWidth.x, inWidth.x, inWidth.z, inWidth.z), vec4(0.0));

    float outerDistance = roundedRectSDF(inPoint, inBox, inRadiusX, inRadiusY);
    float innerDistance = roundedRectSDF(inPoint, innerBox, innerRadiusX, innerRadiusY);
    float outerCoverage = sdCoverage(outerDistance);
    float innerCoverage = sdCoverage(innerDistance);
    float ring          = clamp(outerCoverage - innerCoverage, 0.0, 1.0);

    if (ring <= 0.0) {
        return inFill;
    }

    vec4 borderColor = pickBorderColor(
        inPoint,
        inBox,
        inWidth,
        inColorTop,
        inColorRight,
        inColorBottom,
        inColorLeft
    );

    return mix(inFill, borderColor, ring / max(outerCoverage, 1e-4));
}
