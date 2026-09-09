const int POLY_FLAG_LIT        = 1;
const int POLY_FLAG_SHADOW     = 2;
const int POLY_FLAG_FOREGROUND = 4;
const int POLY_FLAG_OUTLINED   = 8;

struct PolyInstance3D {
    mat4 model;

    ivec4 textures0; // Base, AmbientOcclusion, Normal, Roughness
    ivec4 textures1; // Metalness, Height, Specular, Opacity
    ivec4 textures2; // Refraction, SelfIllumination, Bump, Reflection

    int flags;
    int _pad0;
    int _pad1;
    int _pad2;
};
