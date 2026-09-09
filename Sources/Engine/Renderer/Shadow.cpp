#include "Chicane/Renderer/Shadow.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#include <glm/gtc/matrix_transform.hpp>

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Renderer/Light/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace Shadow
        {
            namespace
            {
                constexpr float kSplitLambda        = 0.75f;
                constexpr float kMaxShadowDistance  = 250.0f;
                constexpr float kDepthPaddingFactor = 1.5f;

                float length(const Vec3& inValue)
                {
                    return std::sqrt(inValue.dot(inValue));
                }

                void getFrustumCorners(const Mat4& inInverseViewProjection, Vec3 outCorners[8])
                {
                    std::uint32_t index = 0;
                    for (std::uint32_t z = 0; z < 2; ++z)
                    {
                        for (std::uint32_t y = 0; y < 2; ++y)
                        {
                            for (std::uint32_t x = 0; x < 2; ++x)
                            {
                                const Vec4 corner = inInverseViewProjection * Vec4(
                                                                                  2.0f * static_cast<float>(x) - 1.0f,
                                                                                  2.0f * static_cast<float>(y) - 1.0f,
                                                                                  2.0f * static_cast<float>(z) - 1.0f,
                                                                                  1.0f
                                                                              );

                                outCorners[index++] = Vec3(corner.x, corner.y, corner.z) * (1.0f / corner.w);
                            }
                        }
                    }
                }

                void buildCascades(ShadowLight& outLight, const View& inCamera, const Vec3& inLightDirection)
                {
                    const float nearClip  = std::max(inCamera.clip.x, 0.1f);
                    const float cameraFar = std::max(inCamera.clip.y, nearClip + 1.0f);
                    const float farClip   = std::max(std::min(cameraFar, kMaxShadowDistance), nearClip + 1.0f);

                    float splits[SHADOW_CASCADE_COUNT] = {};
                    for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; ++cascade)
                    {
                        const float progress =
                            static_cast<float>(cascade + 1) / static_cast<float>(SHADOW_CASCADE_COUNT);
                        const float logSplit = nearClip * std::pow(farClip / nearClip, progress);
                        const float uniSplit = nearClip + (farClip - nearClip) * progress;
                        splits[cascade]      = uniSplit * (1.0f - kSplitLambda) + logSplit * kSplitLambda;
                    }

                    outLight.splits = Vec4(splits[0], splits[1], splits[2], splits[3]);

                    const Mat4 cameraViewProjection  = inCamera.projection * inCamera.view;
                    const Mat4 inverseViewProjection = cameraViewProjection.inverse();
                    Vec3       fullFrustumCorners[8] = {};
                    getFrustumCorners(inverseViewProjection, fullFrustumCorners);

                    Vec3 up = Vec3::Up();
                    if (std::abs(inLightDirection.dot(up)) > 0.999f)
                    {
                        up = Vec3::Right();
                    }

                    const float cameraRange = cameraFar - nearClip;

                    float lastSplit = 0.0f;
                    for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; ++cascade)
                    {
                        const float split      = std::clamp((splits[cascade] - nearClip) / cameraRange, 0.0f, 1.0f);
                        Vec3        corners[8] = {};

                        for (std::uint32_t corner = 0; corner < 4; ++corner)
                        {
                            const Vec3 edge     = fullFrustumCorners[corner + 4] - fullFrustumCorners[corner];
                            corners[corner]     = fullFrustumCorners[corner] + edge * lastSplit;
                            corners[corner + 4] = fullFrustumCorners[corner] + edge * split;
                        }

                        Vec3 center = Vec3::Zero();
                        for (const Vec3& corner : corners)
                        {
                            center += corner;
                        }
                        center = center * (1.0f / 8.0f);

                        float radius = 0.0f;
                        for (const Vec3& corner : corners)
                        {
                            radius = std::max(radius, length(corner - center));
                        }
                        radius = std::ceil(radius * 16.0f) / 16.0f;
                        radius = std::max(radius, 8.0f);

                        const Vec3 eye       = center - inLightDirection * (radius * kDepthPaddingFactor);
                        const Mat4 lightView = glm::lookAt(
                            static_cast<glm::vec3>(eye),
                            static_cast<glm::vec3>(center),
                            static_cast<glm::vec3>(up)
                        );

                        const float diameter            = radius * 2.0f;
                        const float worldUnitsPerTexelX = diameter / static_cast<float>(SHADOW_MAP_WIDTH);
                        const float worldUnitsPerTexelY = diameter / static_cast<float>(SHADOW_MAP_HEIGHT);

                        Vec4 centerLight = lightView * Vec4(center, 1.0f);
                        centerLight.x    = std::floor(centerLight.x / worldUnitsPerTexelX) * worldUnitsPerTexelX;
                        centerLight.y    = std::floor(centerLight.y / worldUnitsPerTexelY) * worldUnitsPerTexelY;

                        const float minX = centerLight.x - radius;
                        const float maxX = centerLight.x + radius;
                        const float minY = centerLight.y - radius;
                        const float maxY = centerLight.y + radius;

                        float minZ = std::numeric_limits<float>::max();
                        float maxZ = -std::numeric_limits<float>::max();

                        for (const Vec3& corner : corners)
                        {
                            const Vec4 lightSpace = lightView * Vec4(corner, 1.0f);
                            minZ                  = std::min(minZ, lightSpace.z);
                            maxZ                  = std::max(maxZ, lightSpace.z);
                        }

                        const float depthPadding = radius * kDepthPaddingFactor;
                        minZ -= depthPadding;
                        maxZ += depthPadding;

                        const float nearPlane = std::max(0.01f, -maxZ);
                        const float farPlane  = std::max(nearPlane + 0.01f, -minZ);

                        outLight.views[cascade]       = lightView;
                        outLight.projections[cascade] = glm::ortho(minX, maxX, minY, maxY, nearPlane, farPlane);

                        lastSplit = split;
                    }
                }

                int findShadowCaster(const Light::List& inLights, std::uint32_t inCount)
                {
                    int directional = -1;
                    int any         = -1;

                    for (std::uint32_t index = 0; index < inCount; ++index)
                    {
                        const Light& light = inLights.at(index);
                        if (!light.castShadows || light.type == LightType::Environment)
                        {
                            continue;
                        }

                        if (any < 0)
                        {
                            any = static_cast<int>(index);
                        }

                        if (light.type == LightType::Directional)
                        {
                            directional = static_cast<int>(index);
                            break;
                        }
                    }

                    return directional >= 0 ? directional : any;
                }

                bool isVisible(const Light& inLight)
                {
                    return inLight.intensity > 0.0f && inLight.color.dot(inLight.color) > 0.0f;
                }

                std::uint32_t getPriority(const Light& inLight)
                {
                    switch (inLight.type)
                    {
                    case LightType::Environment:
                        return 0;

                    case LightType::Directional:
                        return inLight.castShadows ? 1 : 2;

                    default:
                        return inLight.castShadows ? 3 : 4;
                    }
                }

                Light::List selectLights(const View& inCamera, const Light::List& inLights)
                {
                    const Vec3 cameraTranslation =
                        Vec3(inCamera.translation.x, inCamera.translation.y, inCamera.translation.z);

                    std::vector<std::uint32_t> indices;
                    indices.reserve(inLights.size());

                    bool hasEnvironment = false;
                    for (std::uint32_t index = 0; index < inLights.size(); ++index)
                    {
                        const Light& light = inLights.at(index);
                        if (!isVisible(light))
                        {
                            continue;
                        }

                        if (light.type == LightType::Environment)
                        {
                            if (hasEnvironment)
                            {
                                continue;
                            }

                            hasEnvironment = true;
                        }

                        indices.push_back(index);
                    }

                    if (indices.size() > MAX_LIGHTS)
                    {
                        std::stable_sort(
                            indices.begin(),
                            indices.end(),
                            [&](std::uint32_t inLeft, std::uint32_t inRight)
                            {
                                const Light& left  = inLights.at(inLeft);
                                const Light& right = inLights.at(inRight);

                                const std::uint32_t leftPriority  = getPriority(left);
                                const std::uint32_t rightPriority = getPriority(right);
                                if (leftPriority != rightPriority)
                                {
                                    return leftPriority < rightPriority;
                                }

                                return length(left.translation - cameraTranslation) - left.range <
                                       length(right.translation - cameraTranslation) - right.range;
                            }
                        );

                        indices.resize(MAX_LIGHTS);
                    }

                    Light::List result;
                    result.reserve(indices.size());
                    for (std::uint32_t index : indices)
                    {
                        result.push_back(inLights.at(index));
                    }

                    return result;
                }

                Vec4 getCone(const Light& inLight)
                {
                    if (inLight.type != LightType::Spot)
                    {
                        return Vec4::Zero();
                    }

                    constexpr float kDegreesToRadians = 3.14159265358979323846f / 180.0f;

                    const float outer = std::clamp(inLight.outerAngle, 0.0f, 89.9f);
                    const float inner = std::clamp(inLight.innerAngle, 0.0f, outer);

                    const float cosOuter = std::cos(outer * kDegreesToRadians);
                    const float cosInner = std::max(std::cos(inner * kDegreesToRadians), cosOuter + 1e-4f);

                    return Vec4(cosInner, cosOuter, 0.0f, 0.0f);
                }

                int findEnvironmentLight(const Light::List& inLights, std::uint32_t inCount)
                {
                    for (std::uint32_t index = 0; index < inCount; ++index)
                    {
                        if (inLights.at(index).type == LightType::Environment)
                        {
                            return static_cast<int>(index);
                        }
                    }

                    return -1;
                }
            }

            ShadowLight build(const View& inCamera, const Light::List& inLights)
            {
                ShadowLight result = {};
                result.info.z      = -1.0f;
                result.info.w      = -1.0f;
                if (inLights.empty())
                {
                    return result;
                }

                const Light::List   lights = selectLights(inCamera, inLights);
                const std::uint32_t count  = static_cast<std::uint32_t>(lights.size());
                result.info.x              = static_cast<float>(count);

                for (std::uint32_t index = 0; index < count; ++index)
                {
                    const Light& source = lights.at(index);

                    const bool bIsDegenerate = source.direction.dot(source.direction) < 1e-8f;
                    const Vec3 direction     = bIsDegenerate ? Vec3(0.0f, 0.0f, -1.0f) : source.direction.normalize();

                    const Vec3 litColor = source.color * source.intensity;

                    result.lights[index].translation =
                        Vec4(source.translation.x, source.translation.y, source.translation.z, source.range);
                    result.lights[index].direction =
                        Vec4(direction.x, direction.y, direction.z, static_cast<float>(source.type));
                    result.lights[index].color = Vec4(litColor.x, litColor.y, litColor.z, 1.0f);
                    result.lights[index].cone  = getCone(source);
                }

                const int caster = findShadowCaster(lights, count);
                result.info.z    = static_cast<float>(caster);
                result.info.w    = static_cast<float>(findEnvironmentLight(lights, count));

                if (caster >= 0)
                {
                    const Vec3 cascadeDirection = Vec3(
                        result.lights[caster].direction.x,
                        result.lights[caster].direction.y,
                        result.lights[caster].direction.z
                    );
                    buildCascades(result, inCamera, cascadeDirection);
                }

                return result;
            }
        }
    }
}
