struct PolyInstance2D {
    vec2 view;
    vec2 scale;
    vec2 size;
    vec2 offset;
    vec3 position;
    float backdropBlur;
    vec4 color;
    vec4 clip;
    int textureIndex;
    int glyphIndex;
    float glyphDilation;
    float filterBlur;
    vec2 transformX;
    vec2 transformY;
    vec4 radiusX;
    vec4 radiusY;
    vec4 innerClip;
    vec4 innerClipRadiusX;
    vec4 innerClipRadiusY;
    vec4 outerClip;
    vec4 outerClipRadiusX;
    vec4 outerClipRadiusY;
    vec4 borderWidth;
    vec4 borderColorTop;
    vec4 borderColorRight;
    vec4 borderColorBottom;
    vec4 borderColorLeft;
    int gradientType;
    int gradientStopCount;
    vec2 gradientAxis;
    vec4 gradientStops[8];
    vec4 gradientOffsets0;
    vec4 gradientOffsets1;
};

float normalize2DDepth(float inValue) {
    float result = inValue;
    result /= 999.0;
    result  = clamp(abs(result - 1.0), 0.0, 1.0);

    return result;
}

vec2 get2DVertexPosition(PolyInstance2D inInstance, vec2 inPosition, vec2 inMultipliers) {
    vec2 position = inPosition;
    position *= inInstance.scale;  // Apply vertex scale
    position += inInstance.offset; // Apply vertex offset
    position  = (inInstance.transformX * position.x) + (inInstance.transformY * position.y);
    position /= inInstance.view;   // Normalize to view size
    position *= 2.0;               // Normalize to NDC
    position *= inMultipliers;

    return position;
}

vec2 get2DLocalFrag(vec2 inFrag, vec4 inBox, vec4 inLinear) {
    vec2 center = mix(inBox.xy, inBox.zw, 0.5);
    mat2 linear = mat2(inLinear.xy, inLinear.zw);
    float det   = (linear[0][0] * linear[1][1]) - (linear[0][1] * linear[1][0]);

    if (abs(det) < 1e-6) {
        return inFrag;
    }

    return center + (inverse(linear) * (inFrag - center));
}

vec3 get2DScreenPosition(PolyInstance2D inInstance, vec2 inMultipliers) {
    vec3 position = inInstance.position;
    position.xy  += inInstance.size * 0.5;     // Convert to top-left origin
    position.xy  /= inInstance.view;           // Normalize to view size
    position.xy   = (position.xy * 2.0) - 1.0; // Normalize to NDC
    position.xy  *= inMultipliers;
    position.z    = normalize2DDepth(position.z);

    return position;
}

vec4 normalize2DColor(vec4 inColor) {
    return inColor / 255.0;
}

const float FILTER_BLUR_SUPPORT = 3.0;

vec2 get2DBlurredScale(PolyInstance2D inInstance) {
    return inInstance.scale + vec2(inInstance.filterBlur * FILTER_BLUR_SUPPORT * 2.0);
}

vec2 get2DFilterUV(vec2 inUV, vec2 inOriginalScale, vec2 inBlurredScale) {
    vec2 uv = inUV;

    if (inOriginalScale.x > 1e-4) {
        uv.x = ((inUV.x - 0.5) * (inBlurredScale.x / inOriginalScale.x)) + 0.5;
    }

    if (inOriginalScale.y > 1e-4) {
        uv.y = ((inUV.y - 0.5) * (inBlurredScale.y / inOriginalScale.y)) + 0.5;
    }

    return uv;
}

vec4 samplePoly2DGradient(PolyInstance2D inInstance, vec2 inUV) {
    if (inInstance.gradientType <= 0 || inInstance.gradientStopCount <= 0) {
        return normalize2DColor(inInstance.color);
    }

    float t = 0.0;

    if (inInstance.gradientType == 1) {
        vec2  axis  = inInstance.gradientAxis;
        vec2  start = vec2(0.5) - (axis * 0.5);
        float denom = max(dot(axis, axis), 1e-5);
        t = dot(inUV - start, axis) / denom;
    } else {
        vec2  center = inInstance.gradientAxis;
        vec2  extent = max(center, vec2(1.0) - center);
        float radius = max(length(extent), 1e-5);
        t = distance(inUV, center) / radius;
    }

    t = clamp(t, 0.0, 1.0);

    int count = clamp(inInstance.gradientStopCount, 1, 8);
    float offsets[8];
    offsets[0] = inInstance.gradientOffsets0.x;
    offsets[1] = inInstance.gradientOffsets0.y;
    offsets[2] = inInstance.gradientOffsets0.z;
    offsets[3] = inInstance.gradientOffsets0.w;
    offsets[4] = inInstance.gradientOffsets1.x;
    offsets[5] = inInstance.gradientOffsets1.y;
    offsets[6] = inInstance.gradientOffsets1.z;
    offsets[7] = inInstance.gradientOffsets1.w;

    if (t <= offsets[0]) {
        return normalize2DColor(inInstance.gradientStops[0]);
    }

    if (t >= offsets[count - 1]) {
        return normalize2DColor(inInstance.gradientStops[count - 1]);
    }

    for (int i = 0; i < 7; i++) {
        if (i >= count - 1) {
            break;
        }

        float start = offsets[i];
        float end   = offsets[i + 1];

        if (t < start || t > end) {
            continue;
        }

        float span = max(end - start, 1e-5);
        float u    = (t - start) / span;

        return mix(
            normalize2DColor(inInstance.gradientStops[i]),
            normalize2DColor(inInstance.gradientStops[i + 1]),
            u
        );
    }

    return normalize2DColor(inInstance.gradientStops[count - 1]);
}
