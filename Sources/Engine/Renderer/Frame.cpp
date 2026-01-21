#include "Chicane/Renderer/Frame.hpp"

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
            resetDrawResources();
            resetDraw2D();
            resetDraw3D();
        }

        void Frame::setup(const DrawResource::Map& inResources)
        {
            m_drawResources = inResources;
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

        const DrawResource& Frame::getResources2D() const
        {
            return m_drawResources.at(DrawType::e2D);
        }

        const Draw2DInstance::List Frame::getInstances2D() const
        {
            Draw2DInstance::List result;

            for (const auto& [id, instances] : m_draws2D)
            {
                result.insert(result.end(), instances.begin(), instances.end());
            }

            return result;
        }

        const std::uint32_t Frame::getInstance2DStart(Draw::Id inId) const
        {
            std::uint32_t result = 0U;

            for (const auto& [id, instance] : m_draws2D)
            {
                if (id == inId)
                {
                    break;
                }

                result += instance.size();
            }

            return result;
        }

        const std::uint32_t Frame::getInstance2DCount(Draw::Id inId) const
        {
            if (m_draws2D.find(inId) == m_draws2D.end())
            {
                return 0U;
            }

            return m_draws2D.at(inId).size();
        }

        void Frame::use(Draw::Id inId, const Draw2DInstance& inInstance)
        {
            m_draws2D[inId].push_back(inInstance);
        }

        const DrawResource& Frame::getResources3D() const
        {
            return m_drawResources.at(DrawType::e3D);
        }

        const Draw3DInstance::List Frame::getInstances3D() const
        {
            Draw3DInstance::List result;

            for (const auto& [id, instance] : m_draws3D)
            {
                result.insert(result.end(), instance.begin(), instance.end());
            }

            return result;
        }

        const std::uint32_t Frame::getInstance3DStart(Draw::Id inId) const
        {
            std::uint32_t result = 0U;

            for (const auto& [id, instance] : m_draws3D)
            {
                if (id == inId)
                {
                    break;
                }

                result += instance.size();
            }

            return result;
        }

        const std::uint32_t Frame::getInstance3DCount(Draw::Id inId) const
        {
            if (m_draws3D.find(inId) == m_draws3D.end())
            {
                return 0U;
            }

            return m_draws3D.at(inId).size();
        }

        void Frame::use(Draw::Id inId, const Draw3DInstance& inInstance)
        {
            m_draws3D[inId].push_back(inInstance);
        }

        void Frame::resetCamera()
        {
            m_camera = {};
        }

        void Frame::resetLights()
        {
            m_lights.clear();
        }

        void Frame::resetDrawResources()
        {
            m_drawResources.clear();
        }

        void Frame::resetDraw2D()
        {
            m_draws2D.clear();
        }

        void Frame::resetDraw3D()
        {
            m_draws3D.clear();
        }
    }
}