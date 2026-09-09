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
                    const float nearClip = std::max(inCamera.clip.x, 0.1f);
                    const float farClip  = std::max(std::min(inCamera.clip.y, kMaxShadowDistance), nearClip + 1.0f);

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

                    float lastSplit = nearClip / farClip;
                    for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; ++cascade)
                    {
                        const float split      = splits[cascade] / farClip;
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
                            static_cast<glm::vec3>(Vec3::Up())
                        );

                        float minX = std::numeric_limits<float>::max();
                        float maxX = -std::numeric_limits<float>::max();
                        float minY = std::numeric_limits<float>::max();
                        float maxY = -std::numeric_limits<float>::max();
                        float minZ = std::numeric_limits<float>::max();
                        float maxZ = -std::numeric_limits<float>::max();

                        for (const Vec3& corner : corners)
                        {
                            const Vec4 lightSpace = lightView * Vec4(corner, 1.0f);
                            minX                  = std::min(minX, lightSpace.x);
                            maxX                  = std::max(maxX, lightSpace.x);
                            minY                  = std::min(minY, lightSpace.y);
                            maxY                  = std::max(maxY, lightSpace.y);
                            minZ                  = std::min(minZ, lightSpace.z);
                            maxZ                  = std::max(maxZ, lightSpace.z);
                        }

                        const float padX = std::max((maxX - minX) * 0.25f, radius * 0.35f);
                        const float padY = std::max((maxY - minY) * 0.25f, radius * 0.35f);
                        minX -= padX;
                        maxX += padX;
                        minY -= padY;
                        maxY += padY;

                        const float worldUnitsPerTexelX = (maxX - minX) / static_cast<float>(SHADOW_MAP_WIDTH);
                        const float worldUnitsPerTexelY = (maxY - minY) / static_cast<float>(SHADOW_MAP_HEIGHT);
                        if (worldUnitsPerTexelX > 0.0f)
                        {
                            minX = std::floor(minX / worldUnitsPerTexelX) * worldUnitsPerTexelX;
                            maxX = std::ceil(maxX / worldUnitsPerTexelX) * worldUnitsPerTexelX;
                        }
                        if (worldUnitsPerTexelY > 0.0f)
                        {
                            minY = std::floor(minY / worldUnitsPerTexelY) * worldUnitsPerTexelY;
                            maxY = std::ceil(maxY / worldUnitsPerTexelY) * worldUnitsPerTexelY;
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

                const std::uint32_t count = std::min(static_cast<std::uint32_t>(inLights.size()), MAX_LIGHTS);
                result.info.x             = static_cast<float>(count);

                for (std::uint32_t index = 0; index < count; ++index)
                {
                    const Light& source = inLights.at(index);

                    Vec3 direction = source.direction.normalize();
                    if (direction.dot(direction) < 1e-8f)
                    {
                        direction = Vec3(0.0f, 0.0f, -1.0f);
                    }

                    const Vec3 litColor = source.color * source.intensity;

                    result.lights[index].translation =
                        Vec4(source.translation.x, source.translation.y, source.translation.z, source.range);
                    result.lights[index].direction =
                        Vec4(direction.x, direction.y, direction.z, static_cast<float>(source.type));
                    result.lights[index].color = Vec4(litColor.x, litColor.y, litColor.z, 1.0f);
                }

                const int caster = findShadowCaster(inLights, count);
                result.info.z    = static_cast<float>(caster);
                result.info.w    = static_cast<float>(findEnvironmentLight(inLights, count));

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
