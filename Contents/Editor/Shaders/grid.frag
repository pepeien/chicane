#version 450

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform CameraData {
    vec4 clip;

    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    mat4 inversedViewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;

    vec4 translation;
} camera;

layout(location = 0) in vec3 inNearPoint;
layout(location = 1) in vec3 inFarPoint;
layout(location = 2) in float inDepth;

vec4 grid(vec3 position, float scale, bool hasAxis) {
    vec2 coord      = position.xy * scale;
    vec2 derivative = fwidth(coord);
    vec2 uv         = fract(coord - 0.5) - 0.5;
    vec2 grid       = abs(uv) / derivative;

    float line     = min(grid.x, grid.y);
    float minimumx = min(derivative.x, 1.0);
    float minimumy = min(derivative.y, 1.0);

    vec4 color = vec4(0.14);
    color.a    = 1.0 - min(line, 1.0);

    if (hasAxis == false) {
        return color;
    }

    if (position.x > (-0.1 * minimumx) && position.x < (0.1 * minimumx)) {
        color.rgb = vec3(0.94, 0.15, 0.22);
    }

    if (position.y > (-0.1 * minimumy) && position.y < (0.1 * minimumy)) {
        color.rgb = vec3(0.21, 0.21, 0.56);
    }

    return color;
}

float computeDepth(vec4 clipSpacePosition) {
    return clipSpacePosition.y / clipSpacePosition.w;
}

float computeLinearDepth(vec4 clipSpacePosition) {
    float clipSpaceDepth = (clipSpacePosition.y / clipSpacePosition.w) * 2.0 - 1.0;
    float linearDepth = (2.0 * camera.clip.x * camera.clip.y) / (camera.clip.y + camera.clip.x - clipSpaceDepth * (camera.clip.y - camera.clip.x));

    return linearDepth / camera.clip.y; 
}

void main() {
    float floorDistance = -inNearPoint.z / (inFarPoint.z - inNearPoint.z);

    if (floorDistance < 0.0) {
        return;
    }

    vec3 position     = inNearPoint + floorDistance * (inFarPoint - inNearPoint);
    vec4 clipSpace    = camera.inversedViewProjection * vec4(position, 1.0);
    float depth       = computeDepth(clipSpace);
    float linearDepth = computeLinearDepth(clipSpace);

    outColor    = grid(position, 1.0, false);
    outColor   += grid(position, 10.0, true);
    outColor.a *= (1.0 - smoothstep(0.2, 1.0, linearDepth));

    gl_FragDepth = inDepth;
}