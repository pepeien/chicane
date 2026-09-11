const vec2 POISSON_DISK[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2( 0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2( 0.34495938,  0.29387760),
    vec2(-0.91588581,  0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543,  0.27676845),
    vec2( 0.97484398,  0.75648379),
    vec2( 0.44323325, -0.97511554),
    vec2( 0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2( 0.79197514,  0.19048895),
    vec2(-0.24188840,  0.71207465),
    vec2( 0.19984126,  0.78641367),
    vec2(-0.64953355,  0.08202333),
    vec2( 0.20881542, -0.20714308)
);

int selectCascade(float viewDepth, vec4 splits) {
    if (viewDepth < splits.x) {
        return 0;
    }
    if (viewDepth < splits.y) {
        return 1;
    }
    if (viewDepth < splits.z) {
        return 2;
    }
    return 3;
}

float ShadowCalculation(
    sampler2DArray shadowMap,
    int cascadeIndex,
    mat4 cascadeView,
    mat4 cascadeProjection,
    vec3 worldPosition,
    vec3 normal,
    vec3 lightDirection
) {
    vec4 projection = cascadeProjection * cascadeView * vec4(worldPosition, 1.0);
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
    float NdotL        = max(dot(normal, lightDirection), 0.0);
    vec2  texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float bias      = max(texelSize.x * (4.0 - 3.0 * NdotL), texelSize.x * 0.5);

    float filterRadius = 1.5;
    float shadow       = 0.0;

    for (int i = 0; i < 16; ++i) {
        vec2  sampleCoord = projCoords.xy + POISSON_DISK[i] * texelSize * filterRadius;
        float pcfDepth    = texture(shadowMap, vec3(sampleCoord, float(cascadeIndex))).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }

    return shadow / 16.0;
}

vec3 applyTangentNormal(vec3 geometricNormal, vec3 tangentNormal, vec3 worldPosition, vec2 uv) {
    vec3 q1  = dFdx(worldPosition);
    vec3 q2  = dFdy(worldPosition);
    vec2 st1 = dFdx(uv);
    vec2 st2 = dFdy(uv);

#if defined(VULKAN)
    q2  = -q2;
    st2 = -st2;
#endif

    vec3 n = normalize(geometricNormal);
    vec3 t = q1 * st2.t - q2 * st1.t;
    if (dot(t, t) < 1e-8) {
        return n;
    }

    t = normalize(t - n * dot(n, t));
    vec3 b = normalize(cross(n, t));

    return normalize(mat3(t, b, n) * tangentNormal);
}

vec3 resolveLightDirection(vec4 translation, vec4 direction, vec3 worldPosition) {
    float lightType = direction.w;

    if (lightType < 0.5) {
        return normalize(-direction.xyz);
    }

    vec3 toLight = translation.xyz - worldPosition;
    float distanceSq = max(dot(toLight, toLight), 0.00000001);
    return toLight * inversesqrt(distanceSq);
}

float resolveLightAttenuation(vec4 translation, vec4 direction, vec4 cone, vec3 worldPosition) {
    float lightType = direction.w;
    if (lightType < 0.5) {
        return 1.0;
    }

    float range = max(translation.w, 0.001);
    float distance = length(translation.xyz - worldPosition);
    float normalized = clamp(distance / range, 0.0, 1.0);

    float squared = normalized * normalized;
    float attenuation = 1.0 - squared * squared;
    attenuation *= attenuation;

    if (lightType >= 1.5) {
        vec3 toLight = normalize(translation.xyz - worldPosition);
        float cosTheta = dot(toLight, normalize(-direction.xyz));
        attenuation *= smoothstep(cone.y, cone.x, cosTheta);
    }

    return attenuation;
}

vec3 evaluateLightContribution(
    vec3 albedo,
    vec3 normal,
    vec3 lightDirection,
    vec3 viewDirection,
    float roughness,
    float metalness,
    vec3 specularColor,
    float shadow,
    vec3 lightColor
) {
    if (dot(lightDirection, lightDirection) < 0.00000001) {
        return vec3(0.0);
    }

    float alpha     = max(roughness * roughness, 0.002);
    float shininess = max((2.0 / (alpha * alpha)) - 2.0, 1.0);

    vec3 halfway = normalize(lightDirection + viewDirection);
    float NdotL  = max(dot(normal, lightDirection), 0.0);
    float NdotH  = max(dot(normal, halfway), 0.0);
    float VdotH  = max(dot(viewDirection, halfway), 0.0);

    vec3 F0 = mix(vec3(0.04), albedo, metalness) * specularColor;
    vec3 F  = F0 + (vec3(1.0) - F0) * pow(1.0 - VdotH, 5.0);

    float norm     = (shininess + 8.0) / (8.0 * 3.14159265);
    vec3  specular = norm * pow(NdotH, shininess) * F * lightColor * NdotL;
    vec3  diffuse  = (vec3(1.0) - F) * (1.0 - metalness) * NdotL * lightColor;

    return (1.0 - shadow) * (diffuse * albedo + specular);
}

vec3 evaluateIBL(
    samplerCube skyMap,
    vec3 albedo,
    vec3 normal,
    vec3 viewDirection,
    float roughness,
    float metalness,
    vec3 specularColor,
    float ambientOcclusion,
    vec3 environmentColor
) {
    if (dot(environmentColor, environmentColor) < 0.00000001) {
        return vec3(0.0);
    }

    float maxLod = float(textureQueryLevels(skyMap) - 1);

    vec3 irradiance = textureLod(skyMap, normal, maxLod).rgb;
    vec3 reflection = reflect(-viewDirection, normal);
    vec3 specularEnv = textureLod(skyMap, reflection, sqrt(roughness) * maxLod).rgb;

    vec3 F0 = mix(vec3(0.04), albedo, metalness) * specularColor;
    float NdotV = max(dot(normal, viewDirection), 0.0);
    vec3 F = F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - NdotV, 5.0);

    float specularWeight = (1.0 - roughness) * (1.0 - roughness);
    vec3 diffuse  = (1.0 - F) * (1.0 - metalness) * irradiance * albedo;
    vec3 specular = F * mix(irradiance, specularEnv, specularWeight) * specularWeight;

    return (diffuse + specular) * ambientOcclusion * environmentColor;
}
