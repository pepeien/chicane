module Shadow;

static const float2 POISSON_DISK[16] = {
    float2(-0.94201624, -0.39906216),
    float2(0.94558609, -0.76890725),
    float2(-0.094184101, -0.92938870),
    float2(0.34495938, 0.29387760),
    float2(-0.91588581, 0.45771432),
    float2(-0.81544232, -0.87912464),
    float2(-0.38277543, 0.27676845),
    float2(0.97484398, 0.75648379),
    float2(0.44323325, -0.97511554),
    float2(0.53742981, -0.47373420),
    float2(-0.26496911, -0.41893023),
    float2(0.79197514, 0.19048895),
    float2(-0.24188840, 0.71207465),
    float2(0.19984126, 0.78641367),
    float2(-0.64953355, 0.08202333),
    float2(0.20881542, -0.20714308)
};

public int selectCascade(float viewDepth, float4 splits)
{
    if (viewDepth < splits.x)
    {
        return 0;
    }
    if (viewDepth < splits.y)
    {
        return 1;
    }
    if (viewDepth < splits.z)
    {
        return 2;
    }
    return 3;
}

public float ShadowCalculation(
    Sampler2DArray shadowMap,
    int cascadeIndex,
    float4x4 cascadeView,
    float4x4 cascadeProjection,
    float3 worldPosition,
    float3 normal,
    float3 lightDirection
)
{
    float4 projection = mul(cascadeProjection, mul(cascadeView, float4(worldPosition, 1.0)));
    float3 projCoords = projection.xyz / projection.w;
    projCoords.xy     = projCoords.xy * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 || projCoords.z < 0.0 ||
        projCoords.z > 1.0)
    {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float NdotL        = max(dot(normal, lightDirection), 0.0);
    uint width = 1;
    uint height = 1;
    uint elements = 1;
    shadowMap.GetDimensions(width, height, elements);
    float2 texelSize = 1.0 / float2(width, height);
    float bias       = max(texelSize.x * (4.0 - 3.0 * NdotL), texelSize.x * 0.5);

    float filterRadius = 1.5;
    float shadow       = 0.0;

    for (int i = 0; i < 16; ++i)
    {
        float2 sampleCoord = projCoords.xy + POISSON_DISK[i] * texelSize * filterRadius;
        float pcfDepth     = shadowMap.Sample(float3(sampleCoord, float(cascadeIndex))).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }

    return shadow / 16.0;
}

public float3x3 cotangentFrame(float3 geometricNormal, float3 worldPosition, float2 uv)
{
    float3 dp1  = ddx(worldPosition);
    float3 dp2  = ddy(worldPosition);
    float2 duv1 = ddx(uv);
    float2 duv2 = ddy(uv);

#ifdef CHICANE_VULKAN
    dp2  = -dp2;
    duv2 = -duv2;
#endif

    float3 n       = normalize(geometricNormal);
    float3 dp2perp = cross(dp2, n);
    float3 dp1perp = cross(n, dp1);
    float3 T       = dp2perp * duv1.x + dp1perp * duv2.x;
    float3 B       = dp2perp * duv1.y + dp1perp * duv2.y;

    float maxLen = max(dot(T, T), dot(B, B));
    if (maxLen < 1e-12)
    {
        float3 axis = abs(n.z) < 0.9 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
        T           = normalize(cross(axis, n));
        B           = cross(n, T);
        return float3x3(T, B, n);
    }

    float invmax = rsqrt(maxLen);
    T            = normalize(T * invmax);
    B            = normalize(B * invmax);
    if (dot(cross(T, B), n) < 0.0)
    {
        B = -B;
    }

    return float3x3(T, B, n);
}

public float3x3 meshTangentFrame(float3 geometricNormal, float4 vertexTangent, float3 worldPosition, float2 uv)
{
    float3 n = normalize(geometricNormal);
    if (abs(vertexTangent.w) > 0.3 && dot(vertexTangent.xyz, vertexTangent.xyz) > 1e-8)
    {
        float3 T = normalize(vertexTangent.xyz);
        T        = normalize(T - n * dot(n, T));
        float3 B = cross(n, T) * sign(vertexTangent.w);
        return float3x3(T, B, n);
    }

    return cotangentFrame(n, worldPosition, uv);
}

public float3 applyTangentNormal(float3x3 tbn, float3 tangentNormal)
{
    float3 tn = normalize(tangentNormal);
    if (tn.z < 0.0)
    {
        tn.z = abs(tn.z);
    }

    return normalize(mul(tbn, tn));
}

public float3 resolveLightDirection(float4 translation, float4 direction, float3 worldPosition)
{
    float lightType = direction.w;

    if (lightType < 0.5)
    {
        return normalize(-direction.xyz);
    }

    float3 toLight     = translation.xyz - worldPosition;
    float distanceSq = max(dot(toLight, toLight), 0.00000001);
    return toLight * rsqrt(distanceSq);
}

public float resolveLightAttenuation(float4 translation, float4 direction, float4 cone, float3 worldPosition)
{
    float lightType = direction.w;
    if (lightType < 0.5)
    {
        return 1.0;
    }

    float range      = max(translation.w, 0.001);
    float distance   = length(translation.xyz - worldPosition);
    float normalized = saturate(distance / range);

    float squared     = normalized * normalized;
    float attenuation = 1.0 - squared * squared;
    attenuation *= attenuation;

    if (lightType >= 1.5)
    {
        float3 toLight = normalize(translation.xyz - worldPosition);
        float cosTheta = dot(toLight, normalize(-direction.xyz));
        attenuation *= smoothstep(cone.y, cone.x, cosTheta);
    }

    return attenuation;
}

public float3 evaluateLightContribution(
    float3 albedo,
    float3 normal,
    float3 lightDirection,
    float3 viewDirection,
    float roughness,
    float metalness,
    float3 specularColor,
    float shadow,
    float3 lightColor,
    float opacity
)
{
    if (dot(lightDirection, lightDirection) < 0.00000001)
    {
        return float3(0.0, 0.0, 0.0);
    }

    float alpha     = max(roughness * roughness, 0.002);
    float shininess = max((2.0 / (alpha * alpha)) - 2.0, 1.0);

    float3 halfway = normalize(lightDirection + viewDirection);
    float NdotL    = max(dot(normal, lightDirection), 0.0);
    float NdotH    = max(dot(normal, halfway), 0.0);
    float VdotH    = max(dot(viewDirection, halfway), 0.0);

    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metalness) * specularColor;
    float3 F  = F0 + (float3(1.0, 1.0, 1.0) - F0) * pow(1.0 - VdotH, 5.0);

    float norm       = min((shininess + 8.0) / (8.0 * 3.14159265), 8.0);
    float3 specular  = norm * pow(NdotH, shininess) * F * lightColor * NdotL;
    float3 diffuse   = (float3(1.0, 1.0, 1.0) - F) * (1.0 - metalness) * NdotL * lightColor * opacity;

    return (1.0 - shadow) * (diffuse * albedo + specular);
}

public float resolveMeshOpacity(float opacity, float NdotV, int transparentPass)
{
    if (transparentPass < 0)
    {
        return opacity;
    }

    if (transparentPass == 0)
    {
        return opacity < 0.99 ? -1.0 : opacity;
    }

    if (opacity >= 0.99)
    {
        return -1.0;
    }

    if (opacity > 0.95)
    {
        float fresnel = pow(saturate(1.0 - NdotV), 5.0);
        return lerp(0.07, 0.7, fresnel);
    }

    return opacity;
}

public float3 evaluateIBL(
    SamplerCube skyMap,
    float3 albedo,
    float3 normal,
    float3 viewDirection,
    float roughness,
    float metalness,
    float3 specularColor,
    float ambientOcclusion,
    float3 environmentColor,
    float opacity
)
{
    if (dot(environmentColor, environmentColor) < 0.00000001)
    {
        return float3(0.0, 0.0, 0.0);
    }

    uint width = 1;
    uint height = 1;
    skyMap.GetDimensions(width, height);
    float maxLod = max(log2(float(max(width, height))), 0.0);

    float3 irradiance  = skyMap.SampleLevel(normal, maxLod).rgb;
    float3 reflection  = reflect(-viewDirection, normal);
    float3 specularEnv = skyMap.SampleLevel(reflection, sqrt(roughness) * maxLod).rgb;

    float3 F0   = lerp(float3(0.04, 0.04, 0.04), albedo, metalness) * specularColor;
    float NdotV = max(dot(normal, viewDirection), 0.0);
    float3 F    = F0 + (float3(1.0, 1.0, 1.0) - F0) * pow(1.0 - NdotV, 5.0);

    float3 diffuse  = (1.0 - F) * (1.0 - metalness) * irradiance * albedo * opacity;
    float3 specular = F * specularEnv;

    return (diffuse + specular) * ambientOcclusion * environmentColor;
}
