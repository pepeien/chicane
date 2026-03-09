struct CameraInstance {
    vec4 clip;

    mat4 view;
    mat4 projection;

    vec4 up;
    vec4 right;
    vec4 forward;

    vec4 translation;
};

struct LightInstance {
    vec4 clip;

    mat4 view;
    mat4 projection;

    vec4 up;
    vec4 right;
    vec4 forward;

    vec4 translation;
};