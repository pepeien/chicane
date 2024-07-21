#version 450

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    vec4 forward;
    vec4 right;
    vec4 up;
} camera;

layout(location = 0) out vec3 outForward;

const vec2 screen_corners[6] = vec2[](
	vec2(-1.0, -1.0),
	vec2(-1.0,  1.0),
	vec2( 1.0,  1.0),
	vec2( 1.0,  1.0),
	vec2( 1.0, -1.0),
	vec2(-1.0, -1.0)
);

void main() {
    vec2 position = screen_corners[gl_VertexIndex];

    gl_Position = vec4(position, 0.0, 1.0);

    outForward = normalize(
        camera.forward + (position.x * camera.right) - (position.y * camera.up)
    ).xyz;
}