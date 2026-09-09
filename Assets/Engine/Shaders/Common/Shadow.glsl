const vec3 LIGHT_COLOR   = vec3(1.0);
const vec3 AMBIENT_LIGHT = LIGHT_COLOR * 0.3;

float ShadowCalculation(sampler2D shadowMap, vec4 projection, vec3 normal, vec3 lightDirection) {
    vec3 projCoords = projection.xyz / projection.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    if (projCoords.z > 1.0)
    {
        return 0.0;
    }

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

// Cotangent-frame normal mapping when meshes have no tangents.
vec3 applyTangentNormal(vec3 geometricNormal, vec3 tangentNormal, vec3 worldPosition, vec2 uv) {
    vec3 q1  = dFdx(worldPosition);
    vec3 q2  = dFdy(worldPosition);
    vec2 st1 = dFdx(uv);
    vec2 st2 = dFdy(uv);

    vec3 n = normalize(geometricNormal);
    vec3 t = q1 * st2.t - q2 * st1.t;
    if (dot(t, t) < 1e-8) {
        return n;
    }

    t = normalize(t - n * dot(n, t));
    vec3 b = normalize(cross(n, t));

    return normalize(mat3(t, b, n) * tangentNormal);
}

vec3 evaluateLighting(
    vec3 albedo,
    vec3 normal,
    vec3 lightDirection,
    vec3 viewDirection,
    float roughness,
    float metalness,
    vec3 specularColor,
    float ambientOcclusion,
    float shadow
) {
    float alpha     = max(roughness * roughness, 0.002);
    float shininess = max((2.0 / (alpha * alpha)) - 2.0, 1.0);

    vec3 halfway = normalize(lightDirection + viewDirection);
    float NdotL  = max(dot(normal, lightDirection), 0.0);
    float NdotH  = max(dot(normal, halfway), 0.0);
    float VdotH  = max(dot(viewDirection, halfway), 0.0);

    vec3 F0 = mix(vec3(0.04), albedo, metalness) * specularColor;
    vec3 F  = F0 + (vec3(1.0) - F0) * pow(1.0 - VdotH, 5.0);

    float norm     = (shininess + 8.0) / (8.0 * 3.14159265);
    vec3  specular = norm * pow(NdotH, shininess) * F * LIGHT_COLOR * NdotL;
    vec3  diffuse  = (vec3(1.0) - F) * (1.0 - metalness) * NdotL * LIGHT_COLOR;

    return (AMBIENT_LIGHT * ambientOcclusion) * albedo + (1.0 - shadow) * (diffuse * albedo + specular);
}
