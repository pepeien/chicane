struct PolyInstance3D {
    mat4 model;

    ivec4 textures0; // Base, AmbientOcclusion, Normal, Roughness
    ivec4 textures1; // Metalness, Height, Specular, Opacity
    ivec4 textures2; // Refraction, SelfIllumination, Bump, Reflection
};