#version 450

layout(set = 0, binding = 0) uniform CameraUBO {
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

layout(location = 0) out vec3 outForward;

const vec3 positions[6] = vec3[](
	vec3( 1.0,  1.0, 0.0),
	vec3(-1.0, -1.0, 0.0),
	vec3(-1.0,  1.0, 0.0),
	vec3(-1.0, -1.0, 0.0),
	vec3( 1.0,  1.0, 0.0),
	vec3( 1.0, -1.0, 0.0)
);

void main() {
    vec3 position = positions[gl_VertexIndex];

    gl_Position = vec4(position, 1.0);

    outForward = normalize(
        camera.forward + (position.x * camera.right) - (position.y * camera.up)
    ).xyz;
}