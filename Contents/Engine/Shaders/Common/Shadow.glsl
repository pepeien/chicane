const vec3 LIGHT_COLOR   = vec3(1.0);
const vec3 AMBIENT_LIGHT = LIGHT_COLOR * 0.3;

float ShadowCalculation(sampler2D shadowMap, vec4 projection, vec3 normal, vec3 lightDirection) {
    vec3 projCoords = projection.xyz / projection.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    if (
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0
    ) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float diffuse      = dot(normal, -lightDirection);
    float bias         = clamp(mix(0.01, 0.001, diffuse), 0.001, 0.01);

    float shadow    = 0.0;
    vec2 texelSize  = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            vec2 sampleCoord = clamp(projCoords.xy + offset, 0.0, 1.0);
            float pcfDepth = texture(shadowMap, sampleCoord).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}