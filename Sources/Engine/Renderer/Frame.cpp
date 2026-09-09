#include "Chicane/Renderer/Frame.hpp"

#include <algorithm>

#include "Chicane/Renderer/Light/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        void Frame::reset()
        {
            // View
            resetCamera();
            resetLights();

            // Draw
            reset2DDraws();
            reset3DDraws();
        }

        void Frame::setup(const DrawPolyResource::Map& inResources)
        {
            for (const auto& [type, resource] : inResources)
            {
                DrawPoly::List& draws = m_polys[type];

                for (const auto& [reference, draw] : resource.getDraws())
                {
                    draws.push_back(draw);
                }
            }

            refresh2DDraws();
            refresh3DDraws();
        }

        void Frame::setup(const DrawSkyResource& inResource)
        {
            m_skyInstance.model = inResource.getDraw().model;
        }

        const View& Frame::getCamera() const
        {
            return m_camera;
        }

        void Frame::useCamera(const View& inData)
        {
            m_camera = std::move(inData);
        }

        const Light::List& Frame::getLights() const
        {
            return m_lights;
        }

        void Frame::addLight(const Light::List& inData)
        {
            m_lights.insert(m_lights.begin(), inData.begin(), inData.end());
        }

        void Frame::addLight(const Light& inData)
        {
            m_lights.push_back(std::move(inData));
        }

        bool Frame::hasDraws(DrawPolyType inType, DrawPolyMode inMode) const
        {
            return !getDraws(inType, inMode).empty();
        }

        DrawPoly::List Frame::getDraws(DrawPolyType inType, DrawPolyMode inMode) const
        {
            if (inType == DrawPolyType::e2D)
            {
                DrawPoly::List result;
                for (const DrawPoly& draw : m_2DBatches)
                {
                    if (draw.mode != inMode || draw.instanceCount <= 0)
                    {
                        continue;
                    }

                    result.push_back(draw);
                }

                return result;
            }

            if (m_polys.find(inType) == m_polys.end())
            {
                return {};
            }

            DrawPoly::List result;
            for (const DrawPoly& draw : m_polys.at(inType))
            {
                if (draw.mode != inMode || draw.instanceCount <= 0)
                {
                    continue;
                }

                result.emplace_back(std::move(draw));
            }

            return result;
        }

        bool Frame::hasShadowDraws() const
        {
            return anyInstance3D(
                [](const DrawPoly3DInstance& inInstance)
                { return inInstance.has(DrawPoly3DFlag::Shadow) && !inInstance.has(DrawPoly3DFlag::Foreground); }
            );
        }

        DrawPoly::List Frame::getShadowDraws() const
        {
            return splitDraws(
                [](const DrawPoly3DInstance& inInstance)
                { return inInstance.has(DrawPoly3DFlag::Shadow) && !inInstance.has(DrawPoly3DFlag::Foreground); }
            );
        }

        bool Frame::hasSceneDraws() const
        {
            return anyInstance3D([](const DrawPoly3DInstance& inInstance)
                                 { return !inInstance.has(DrawPoly3DFlag::Foreground); });
        }

        DrawPoly::List Frame::getSceneDraws() const
        {
            return splitDraws([](const DrawPoly3DInstance& inInstance)
                              { return !inInstance.has(DrawPoly3DFlag::Foreground); });
        }

        bool Frame::hasForegroundDraws() const
        {
            return anyInstance3D([](const DrawPoly3DInstance& inInstance)
                                 { return inInstance.has(DrawPoly3DFlag::Foreground); });
        }

        DrawPoly::List Frame::getForegroundDraws() const
        {
            return splitDraws([](const DrawPoly3DInstance& inInstance)
                              { return inInstance.has(DrawPoly3DFlag::Foreground); });
        }

        bool Frame::hasOutlineDraws() const
        {
            return anyInstance3D(
                [](const DrawPoly3DInstance& inInstance)
                { return inInstance.has(DrawPoly3DFlag::Outlined) && !inInstance.has(DrawPoly3DFlag::Foreground); }
            );
        }

        DrawPoly::List Frame::getOutlineDraws() const
        {
            return splitDraws(
                [](const DrawPoly3DInstance& inInstance)
                { return inInstance.has(DrawPoly3DFlag::Outlined) && !inInstance.has(DrawPoly3DFlag::Foreground); }
            );
        }

        bool Frame::anyInstance3D(bool (*inPredicate)(const DrawPoly3DInstance&)) const
        {
            return std::any_of(m_3DInstancesFlat.begin(), m_3DInstancesFlat.end(), inPredicate);
        }

        DrawPoly::List Frame::splitDraws(bool (*inPredicate)(const DrawPoly3DInstance&)) const
        {
            DrawPoly::List result;

            for (const DrawPoly& draw : getDraws(DrawPolyType::e3D, DrawPolyMode::Fill))
            {
                std::uint32_t runStart = 0U;
                std::uint32_t runCount = 0U;

                for (std::uint32_t index = 0U; index < draw.instanceCount; ++index)
                {
                    const std::uint32_t instanceIndex = draw.instanceStart + index;

                    const bool bIsMatch =
                        instanceIndex < m_3DInstancesFlat.size() && inPredicate(m_3DInstancesFlat.at(instanceIndex));

                    if (bIsMatch)
                    {
                        if (runCount == 0U)
                        {
                            runStart = instanceIndex;
                        }
                        runCount++;
                        continue;
                    }

                    if (runCount == 0U)
                    {
                        continue;
                    }

                    DrawPoly split      = draw;
                    split.instanceStart = runStart;
                    split.instanceCount = runCount;
                    result.push_back(split);

                    runCount = 0U;
                }

                if (runCount == 0U)
                {
                    continue;
                }

                DrawPoly split      = draw;
                split.instanceStart = runStart;
                split.instanceCount = runCount;
                result.push_back(split);
            }

            return result;
        }

        bool Frame::hasShadowCasterLights() const
        {
            for (const Light& light : m_lights)
            {
                if (light.castShadows && light.type != LightType::Environment)
                {
                    return true;
                }
            }

            return false;
        }

        const DrawPoly2DInstance::List& Frame::getInstances2D() const
        {
            return m_2DInstances;
        }

        void Frame::draw(Draw::Id inId, const DrawPoly2DInstance& inInstance)
        {
            if (inId <= Draw::InvalidId)
            {
                return;
            }

            if (m_2DBatches.empty() || m_2DBatches.back().id != inId)
            {
                DrawPoly batch;
                batch.id            = inId;
                batch.instanceStart = static_cast<std::uint32_t>(m_2DInstances.size());
                batch.instanceCount = 0U;

                m_2DBatches.push_back(batch);
            }

            m_2DInstances.push_back(inInstance);
            m_2DBatches.back().instanceCount++;
        }

        const DrawPoly3DInstance::List& Frame::getInstances3D() const
        {
            return m_3DInstancesFlat;
        }

        void Frame::draw(Draw::Id inId, const DrawPoly3DInstance& inInstance)
        {
            if (inId <= Draw::InvalidId)
            {
                return;
            }

            m_3DInstances[inId].push_back(inInstance);
            rebuildInstances3D();
        }

        const DrawSkyInstance& Frame::getSkyInstance() const
        {
            return m_skyInstance;
        }

        void Frame::resetCamera()
        {
            m_camera = {};
        }

        void Frame::resetLights()
        {
            m_lights.clear();
        }

        void Frame::refresh2DDraws()
        {
            for (DrawPoly& batch : m_2DBatches)
            {
                for (const DrawPoly& poly : m_polys[DrawPolyType::e2D])
                {
                    if (poly.id != batch.id)
                    {
                        continue;
                    }

                    const std::uint32_t instanceStart = batch.instanceStart;
                    const std::uint32_t instanceCount = batch.instanceCount;

                    batch               = poly;
                    batch.instanceStart = instanceStart;
                    batch.instanceCount = instanceCount;

                    break;
                }
            }
        }

        void Frame::reset2DDraws()
        {
            m_polys[DrawPolyType::e2D].clear();
            m_2DBatches.clear();
            m_2DInstances.clear();
        }

        void Frame::refresh3DDraws()
        {
            std::uint32_t start = 0U;
            for (const auto& [id, instances] : m_3DInstances)
            {
                if (instances.empty())
                {
                    continue;
                }

                for (DrawPoly& draw : m_polys[DrawPolyType::e3D])
                {
                    if (draw.id != id)
                    {
                        continue;
                    }

                    draw.instanceStart = start;
                    draw.instanceCount = instances.size();
                }

                start += instances.size();
            }
        }

        void Frame::reset3DDraws()
        {
            m_polys[DrawPolyType::e3D].clear();
            m_3DInstances.clear();
            m_3DInstancesFlat.clear();
        }

        void Frame::rebuildInstances3D()
        {
            m_3DInstancesFlat.clear();

            for (const auto& [id, instances] : m_3DInstances)
            {
                m_3DInstancesFlat.insert(m_3DInstancesFlat.end(), instances.begin(), instances.end());
            }
        }
    }
}
