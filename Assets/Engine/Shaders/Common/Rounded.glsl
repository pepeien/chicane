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
