#version 450

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

layout(location = 0) out vec3 outNearPoint;
layout(location = 1) out vec3 outFarPoint;
layout(location = 2) out float outDepth;

const vec3 positions[4] = vec3[](
	vec3(-1.0, -1.0, 0.0),
    vec3( 1.0, -1.0, 0.0),
    vec3( 1.0,  1.0, 0.0),
    vec3(-1.0,  1.0, 0.0) 
);

const int indices[6] = int[6](0, 2, 1, 2, 0, 3);

vec3 unprojectPoint(float x, float y, float z) {
    vec4 point = camera.inversedViewProjection * vec4(x, y, z, 1.0);

    return point.xyz / point.w;
}

void main() {
    vec3 point = positions[indices[gl_VertexIndex]];

    outDepth = 0.0f;

    if (camera.translation.z > 0.0f) {
        outDepth = 0.9999f;
    }

    // Points
    outNearPoint = unprojectPoint(
        point.x,
        point.y,
        0.0
    ).xyz;
    outFarPoint = unprojectPoint(
        point.x,
        point.y,
        1.0
    ).xyz;

    gl_Position = vec4(point, 1.0);
}