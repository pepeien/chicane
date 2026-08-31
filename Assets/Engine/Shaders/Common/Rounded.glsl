bool hasRoundedCorners(vec4 inRadiusX, vec4 inRadiusY) {
    return max(max(inRadiusX.x, inRadiusX.y), max(inRadiusX.z, inRadiusX.w)) > 0.0 ||
           max(max(inRadiusY.x, inRadiusY.y), max(inRadiusY.z, inRadiusY.w)) > 0.0;
}

bool insideRoundedRect(vec2 inPoint, vec4 inBox, vec4 inRadiusX, vec4 inRadiusY) {
    if (inPoint.x < inBox.x || inPoint.y < inBox.y || inPoint.x > inBox.z || inPoint.y > inBox.w) {
        return false;
    }

    if (!hasRoundedCorners(inRadiusX, inRadiusY)) {
        return true;
    }

    vec2 local = inPoint - inBox.xy;
    vec2 size  = inBox.zw - inBox.xy;
    vec2 radius = vec2(0.0);
    vec2 center = vec2(0.0);
    bool corner = false;

    if (local.x < inRadiusX.x && local.y < inRadiusY.x) {
        radius = vec2(inRadiusX.x, inRadiusY.x);
        center = radius;
        corner = radius.x > 0.0 && radius.y > 0.0;
    } else if (local.x > size.x - inRadiusX.y && local.y < inRadiusY.y) {
        radius = vec2(inRadiusX.y, inRadiusY.y);
        center = vec2(size.x - radius.x, radius.y);
        corner = radius.x > 0.0 && radius.y > 0.0;
    } else if (local.x > size.x - inRadiusX.z && local.y > size.y - inRadiusY.z) {
        radius = vec2(inRadiusX.z, inRadiusY.z);
        center = vec2(size.x - radius.x, size.y - radius.y);
        corner = radius.x > 0.0 && radius.y > 0.0;
    } else if (local.x < inRadiusX.w && local.y > size.y - inRadiusY.w) {
        radius = vec2(inRadiusX.w, inRadiusY.w);
        center = vec2(radius.x, size.y - radius.y);
        corner = radius.x > 0.0 && radius.y > 0.0;
    }

    if (!corner) {
        return true;
    }

    vec2 offset = (local - center) / radius;

    return dot(offset, offset) <= 1.0;
}

float roundedRectCoverage(vec2 inPoint, vec4 inBox, vec4 inRadiusX, vec4 inRadiusY) {
    if (inPoint.x < inBox.x || inPoint.y < inBox.y || inPoint.x > inBox.z || inPoint.y > inBox.w) {
        return 0.0;
    }

    if (!hasRoundedCorners(inRadiusX, inRadiusY)) {
        return 1.0;
    }

    vec2 local = inPoint - inBox.xy;
    vec2 size  = inBox.zw - inBox.xy;
    vec2 radius = vec2(0.0);
    vec2 center = vec2(0.0);
    bool corner = false;

    if (local.x < inRadiusX.x && local.y < inRadiusY.x) {
        radius = vec2(inRadiusX.x, inRadiusY.x);
        center = radius;
        corner = radius.x > 0.0 && radius.y > 0.0;
    } else if (local.x > size.x - inRadiusX.y && local.y < inRadiusY.y) {
        radius = vec2(inRadiusX.y, inRadiusY.y);
        center = vec2(size.x - radius.x, radius.y);
        corner = radius.x > 0.0 && radius.y > 0.0;
    } else if (local.x > size.x - inRadiusX.z && local.y > size.y - inRadiusY.z) {
        radius = vec2(inRadiusX.z, inRadiusY.z);
        center = vec2(size.x - radius.x, size.y - radius.y);
        corner = radius.x > 0.0 && radius.y > 0.0;
    } else if (local.x < inRadiusX.w && local.y > size.y - inRadiusY.w) {
        radius = vec2(inRadiusX.w, inRadiusY.w);
        center = vec2(radius.x, size.y - radius.y);
        corner = radius.x > 0.0 && radius.y > 0.0;
    }

    if (!corner) {
        return 1.0;
    }

    vec2  offset = (local - center) / radius;
    float dist   = (length(offset) - 1.0) * min(radius.x, radius.y);

    return 1.0 - smoothstep(-0.75, 0.75, dist);
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

    radius = min(max(radius, vec2(0.0)), halfSize);

    vec2 q      = abs(p);
    vec2 inner  = halfSize - radius;
    vec2 corner = q - inner;

    if (radius.x > 0.0 && radius.y > 0.0 && corner.x > 0.0 && corner.y > 0.0) {
        vec2 n = corner / radius;

        return (length(n) - 1.0) * min(radius.x, radius.y);
    }

    vec2 d = q - halfSize;

    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

vec4 pickBorderColor(vec2 inPoint, vec4 inBox, vec4 inColorTop, vec4 inColorRight, vec4 inColorBottom, vec4 inColorLeft) {
    vec2 size = max(inBox.zw - inBox.xy, vec2(1e-4));
    vec2 local = inPoint - inBox.xy;
    float dTop = local.y;
    float dRight = size.x - local.x;
    float dBottom = size.y - local.y;
    float dLeft = local.x;
    float nearest = dTop;
    vec4 color = inColorTop;

    if (dRight < nearest) {
        nearest = dRight;
        color = inColorRight;
    }

    if (dBottom < nearest) {
        nearest = dBottom;
        color = inColorBottom;
    }

    if (dLeft < nearest) {
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
    float innerCoverage = roundedRectCoverage(inPoint, innerBox, innerRadiusX, innerRadiusY);
    float ring = 1.0 - innerCoverage;

    if (ring <= 0.0) {
        return inFill;
    }

    vec4 borderColor = pickBorderColor(inPoint, inBox, inColorTop, inColorRight, inColorBottom, inColorLeft);

    return mix(inFill, borderColor, ring);
}
