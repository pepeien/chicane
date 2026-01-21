#include "Chicane/Renderer/Frame.hpp"

#include <algorithm>

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
                DrawPoly::List& draws = m_draws[type];
                draws.insert(draws.end(), resource.getDraws().begin(), resource.getDraws().end());

                std::sort(draws.begin(), draws.end(), [](const Draw& inA, const Draw& inB) {
                    return inA.id <= inB.id;
                });
            }

            refresh2DDraws();
            refresh3DDraws();
        }

        const View& Frame::getCamera() const
        {
            return m_camera;
        }

        void Frame::useCamera(const View& inData)
        {
            m_camera = std::move(inData);
        }

        const View::List& Frame::getLights() const
        {
            return m_lights;
        }

        void Frame::addLights(const View::List& inData)
        {
            m_lights.insert(m_lights.begin(), inData.begin(), inData.end());
        }

        void Frame::addLight(const View& inData)
        {
            m_lights.push_back(std::move(inData));
        }

        const DrawPoly::List& Frame::get2DDraws() const
        {
            return m_draws.at(DrawPolyType::e2D);
        }

        const DrawPoly2DInstance::List Frame::getInstances2D() const
        {
            DrawPoly2DInstance::List result;

            for (const auto& [id, instances] : m_2DDrawInstances)
            {
                result.insert(result.end(), instances.begin(), instances.end());
            }

            return result;
        }

        void Frame::use(Draw::Id inId, const DrawPoly2DInstance& inInstance)
        {
            m_2DDrawInstances[inId].push_back(inInstance);

            refresh2DDraws();
        }

        const DrawPoly::List& Frame::get3DDraws() const
        {
            return m_draws.at(DrawPolyType::e3D);
        }

        const DrawPoly3DInstance::List Frame::getInstances3D() const
        {
            DrawPoly3DInstance::List result;

            for (const auto& [id, instance] : m_3DDrawInstances)
            {
                result.insert(result.end(), instance.begin(), instance.end());
            }

            return result;
        }

        void Frame::use(Draw::Id inId, const DrawPoly3DInstance& inInstance)
        {
            m_3DDrawInstances[inId].push_back(inInstance);

            refresh3DDraws();
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
            std::uint32_t start = 0U;
            for (const auto& [id, instances] : m_2DDrawInstances)
            {
                if (instances.empty())
                {
                    continue;
                }

                for (DrawPoly& draw : m_draws[DrawPolyType::e2D])
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

        void Frame::reset2DDraws()
        {
            m_draws[DrawPolyType::e2D].clear();
            m_2DDrawInstances.clear();
        }

        void Frame::refresh3DDraws()
        {
            std::uint32_t start = 0U;
            for (const auto& [id, instances] : m_3DDrawInstances)
            {
                if (instances.empty())
                {
                    continue;
                }

                for (DrawPoly& draw : m_draws[DrawPolyType::e3D])
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
            m_draws[DrawPolyType::e3D].clear();
            m_3DDrawInstances.clear();
        }
    }
}