#ifndef CHICANE_TRIPLANAR_GLSL
#define CHICANE_TRIPLANAR_GLSL

vec3 triplanarBlend(vec3 geometricNormal) {
    vec3 blend = pow(abs(normalize(geometricNormal)), vec3(4.0));
    float sum  = blend.x + blend.y + blend.z;

    return blend / max(sum, 1e-5);
}

vec2 triplanarUvX(vec3 worldPos, float tileSize) {
    return worldPos.zy / tileSize;
}

vec2 triplanarUvY(vec3 worldPos, float tileSize) {
    return worldPos.xz / tileSize;
}

vec2 triplanarUvZ(vec3 worldPos, float tileSize) {
    return worldPos.xy / tileSize;
}

#endif
