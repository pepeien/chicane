import LightInstance;
import SceneLight;
import Poly3D;
import Shadow;
import Triplanar;
import Texture;

[vk::binding(1, 0)] StructuredBuffer<LightInstance> gLights;
[vk::binding(3, 0)] Sampler2DArray gShadowMap;
[vk::binding(4, 0)] SamplerCube gSkyMap;

struct MeshPass
{
    int transparentPass;
}

#ifdef CHICANE_VULKAN
[vk::push_constant]
#else
[vk::binding(15, 0)]
#endif
ConstantBuffer<MeshPass> meshPass;

struct VSOut
{
    nointerpolation int4 textureMaps0 : TEXCOORD0;
    nointerpolation int4 textureMaps1 : TEXCOORD1;
    nointerpolation int4 textureMaps2 : TEXCOORD2;
    nointerpolation int bIsLit : TEXCOORD3;
    nointerpolation float emissiveStrength : TEXCOORD4;
    nointerpolation float tileSize : TEXCOORD5;
    float2 uv : TEXCOORD6;
    float3 normal : TEXCOORD7;
    float4 tangent : TEXCOORD8;
    float4 color : COLOR;
    float3 meshPosition : TEXCOORD9;
    float3 cameraPosition : TEXCOORD10;
    float viewDepth : TEXCOORD11;
}

float4 sampleMap(VSOut inData, int index, float2 uv, float lodBias)
{
    return sampleTextureSlot(index, uv, lodBias);
}

float4 sampleMap(VSOut inData, int index, float2 uv)
{
    float tileSize = inData.tileSize;
    if (tileSize <= 0.0001)
    {
        return sampleMap(inData, index, uv, 0.0);
    }

    float3 worldPos = inData.meshPosition;
    float3 blend    = triplanarBlend(inData.normal);

    return sampleMap(inData, index, triplanarUvX(worldPos, tileSize), 0.0) * blend.x
         + sampleMap(inData, index, triplanarUvY(worldPos, tileSize), 0.0) * blend.y
         + sampleMap(inData, index, triplanarUvZ(worldPos, tileSize), 0.0) * blend.z;
}

[shader("fragment")]
float4 main(VSOut inData) : SV_Target
{
    int baseIndex             = inData.textureMaps0.x;
    int ambientOcclusionIndex = inData.textureMaps0.y;
    int normalIndex           = inData.textureMaps0.z;
    int roughnessIndex        = inData.textureMaps0.w;
    int metalnessIndex        = inData.textureMaps1.x;
    int specularIndex         = inData.textureMaps1.z;
    int opacityIndex          = inData.textureMaps1.w;
    int selfIlluminationIndex = inData.textureMaps2.y;

    float2 uv = inData.uv;

    float4 baseSample      = sampleMap(inData, baseIndex, uv);
    float3 color           = baseSample.rgb * inData.color.rgb;
    float ambientOcclusion = sampleMap(inData, ambientOcclusionIndex, uv).r;
    float roughness        = roughnessIndex < 0 ? 0.9 : sampleMap(inData, roughnessIndex, uv).g;
    float metalness        = metalnessIndex < 0 ? 0.0 : sampleMap(inData, metalnessIndex, uv).b;
    float opacity          = inData.color.a * (opacityIndex < 0 ? baseSample.a : sampleMap(inData, opacityIndex, uv).r);
    float3 emissive        = selfIlluminationIndex < 0 ? float3(0.0, 0.0, 0.0) : sampleMap(inData, selfIlluminationIndex, uv).rgb;
    float3 specularColor   = specularIndex < 0 ? float3(1.0, 1.0, 1.0) : sampleMap(inData, specularIndex, uv).rgb;

    if (opacity < 0.01)
    {
        discard;
    }

    float3 geometricNormal = normalize(inData.normal);
    float3 viewDirection   = normalize(inData.cameraPosition - inData.meshPosition);
    float NdotV            = max(dot(geometricNormal, viewDirection), 0.0);

    opacity = resolveMeshOpacity(opacity, NdotV, meshPass.transparentPass);
    if (opacity < 0.0)
    {
        discard;
    }

    if (inData.bIsLit == 0)
    {
        return float4(color + emissive * inData.emissiveStrength, opacity);
    }

    float3 normal = geometricNormal;
    if (normalIndex >= 0 && inData.tileSize <= 0.0001)
    {
        float3x3 tbn         = meshTangentFrame(geometricNormal, inData.tangent, inData.meshPosition, uv);
        float3 tangentNormal = sampleMap(inData, normalIndex, uv).xyz * 2.0 - 1.0;
        normal               = applyTangentNormal(tbn, tangentNormal);
    }

    int lightCount             = int(gLights[0].info.x);
    int shadowCaster           = int(gLights[0].info.z);
    int environment            = int(gLights[0].info.w);
    float roughnessClamped     = saturate(roughness);
    float metalnessClamped     = saturate(metalness);

    float3 environmentColor = float3(0.0, 0.0, 0.0);
    if (environment >= 0 && environment < lightCount)
    {
        environmentColor = gLights[0].lights[environment].color.rgb;
    }

    float3 lighting = evaluateIBL(
        gSkyMap,
        color,
        normal,
        viewDirection,
        roughnessClamped,
        metalnessClamped,
        specularColor,
        ambientOcclusion,
        environmentColor,
        opacity
    );

    for (int index = 0; index < 8; ++index)
    {
        if (index >= lightCount)
        {
            break;
        }

        SceneLight entry = gLights[0].lights[index];
        if (entry.direction.w > 2.5)
        {
            continue;
        }

        float3 lightDirection = resolveLightDirection(entry.translation, entry.direction, inData.meshPosition);
        float attenuation     = resolveLightAttenuation(entry.translation, entry.direction, entry.cone, inData.meshPosition);
        if (attenuation <= 0.0)
        {
            continue;
        }

        float3 lightColor = entry.color.rgb * attenuation;

        float shadow = 0.0;
        if (index == shadowCaster)
        {
            int cascade = selectCascade(inData.viewDepth, gLights[0].splits);
            shadow      = ShadowCalculation(
                gShadowMap,
                cascade,
                gLights[0].views[cascade],
                gLights[0].projections[cascade],
                inData.meshPosition,
                normal,
                lightDirection
            );
        }

        lighting += evaluateLightContribution(
            color,
            normal,
            lightDirection,
            viewDirection,
            roughnessClamped,
            metalnessClamped,
            specularColor,
            shadow,
            lightColor,
            opacity
        );
    }

    lighting += emissive * inData.emissiveStrength;

    return float4(lighting, opacity);
}
