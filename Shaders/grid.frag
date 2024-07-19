#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in float inNear;
layout(location = 1) in float inFar;
layout(location = 2) in vec3 inNearPoint;
layout(location = 3) in vec3 inFarPoint;
layout(location = 4) in mat4 inView;
layout(location = 8) in mat4 inProjection;

vec4 grid(vec3 position, float scale, bool hasAxis) {
    vec2 coord      = position.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 uv         = fract(coord - 0.5) - 0.5;
    vec2 grid       = abs(uv) / derivative;

    float line     = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);

    vec4 color = vec4(0.3);
    color.a    = 1.0 - min(line, 1.0);

    if (!hasAxis) {
        return color;
    }

    if ((-1.0 * minimumx) < position.x && position.x < (0.1 * minimumx)) {
        color.rgb = vec3(1.0, 0.0, 0.0);
    }

    if ((-1.0 * minimumz) < position.z && position.z < (0.1 * minimumz)) {
        color.rgb = vec3(0.0, 1.0, 0.0);
    }

    return color;
}

float computeDepth(vec3 position) {
    vec4 result = inProjection * inView * vec4(position, 1.0);

    return result.z / result.w;
}

float computeLinearDepth(float depth) {
    float clip_space_depth = depth * 2.0 - 1.0;
    float linearDepth      = (2.0 * inNear * inFar) / (inFar + inNear - clip_space_depth * (inFar - inNear));

    return linearDepth / inFar;
}

void main() {
    float t = -inNearPoint.y / (inFarPoint.y - inNearPoint.y);

    vec3 position = inNearPoint + (t * (inFarPoint - inNearPoint));

    float depth       = computeDepth(position);
    float linearDepth = computeLinearDepth(depth);
    float fade        = max(0.1, 0.25 - linearDepth);

    gl_FragDepth = depth;

    outColor = grid(position, 1, true);
    outColor *= float(t > 0);
    outColor *= fade;

    if(outColor.a == 0.0)
    {
        discard;
    }
}