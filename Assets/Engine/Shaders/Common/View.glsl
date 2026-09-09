struct CameraInstance {
    vec4 clip;

    mat4 view;
    mat4 projection;

    vec4 up;
    vec4 right;
    vec4 forward;

    vec4 translation;
};

struct SceneLight {
    vec4 translation;
    vec4 direction;
    vec4 color;
    vec4 cone;
};

struct LightInstance {
    vec4 splits;
    vec4 info;

    mat4 views[4];
    mat4 projections[4];

    SceneLight lights[8];
};
