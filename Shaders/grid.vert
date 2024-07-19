#version 450

layout(set = 0, binding = 0) uniform UniformObject {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
} camera;

layout(location = 0) out float outNear;
layout(location = 1) out float outFar;
layout(location = 2) out vec3 outNearPoint;
layout(location = 3) out vec3 outFarPoint;
layout(location = 4) out mat4 outView;
layout(location = 8) out mat4 outProjection;

vec3 points[6] = vec3[] (
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 unprojectPoint(float x, float y, float z) {
    vec4 unprojectedPoint =  inverse(camera.viewProjection) * vec4(x, y, z, 1.0);

    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 point = points[gl_VertexIndex].xyz;

    // Values
    outNear = 0.01;
    outFar  = 1000.0;

    // Points
    outNearPoint = unprojectPoint(
        point.x,
        point.y,
        -1.0
    ).xyz;
    outFarPoint  = unprojectPoint(
        point.x,
        point.y,
        1.0
    ).xyz;

    // Camera
    outView       = camera.view;
    outProjection = camera.projection;

    gl_Position = vec4(point, 1.0);
}