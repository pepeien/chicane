float ShadowCalculation(sampler2D shadowMap, vec4 projection, vec3 normal, vec3 lightDirection)
{
    vec3 projCoords = projection.xyz / projection.w;

    #ifdef OPENGL
        projCoords = projCoords * 0.5 + 0.5; 
    #endif

    #ifdef VULKAN
        projCoords.y = 1.0 - projCoords.y;
    #endif

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.002 * (1.0 - dot(normal, lightDirection)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}