#include "Chicane/Renderer/Layer.hpp"

#include "Chicane/Renderer/Backend.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Layer::Layer(const Id& inId)
            : m_id(inId),
              m_status(LayerStatus::Offline),
              m_viewport({}),
              m_backend(nullptr)
        {}

        Layer::Layer()
            : m_id("Undefined"),
              m_status(LayerStatus::Offline),
              m_viewport({}),
              m_backend(nullptr)
        {}

        void Layer::init()
        {
            if (!is(LayerStatus::Offline))
            {
                return;
            }

            if (!onInit())
            {
                return;
            }

            setStatus(LayerStatus::Running);
        }

        void Layer::destroy()
        {
            if (is(LayerStatus::Offline))
            {
                return;
            }

            if (!onDestroy())
            {
                return;
            }

            setStatus(LayerStatus::Offline);
        }

        void Layer::rebuild()
        {
            if (!is(LayerStatus::Offline))
            {
                return;
            }

            if (!onRebuild())
            {
                return;
            }

            setStatus(LayerStatus::Running);
        }

        void Layer::resize(const Vec<2, std::uint32_t>& inResolution)
        {
            if (is(LayerStatus::Offline))
            {
                return;
            }

            onResize(inResolution);
        }

        void Layer::load(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (is(LayerStatus::Offline))
            {
                return;
            }

            onLoad(inType, inResource);
        }

        void Layer::load(const DrawTextureResource& inResources)
        {
            if (is(LayerStatus::Offline))
            {
                return;
            }

            onLoad(inResources);
        }

        void Layer::load(const DrawSky& inResource)
        {
            if (is(LayerStatus::Offline))
            {
                return;
            }

            onLoad(inResource);
        }

        bool Layer::setup(const Frame& inFrame)
        {
            if (!is(LayerStatus::Running))
            {
                return false;
            }

            if (!onSetup(inFrame))
            {
                return false;
            }

            setStatus(LayerStatus::Running);

            return true;
        }

        void Layer::render(const Frame& inFrame, void* inData)
        {
            if (!is(LayerStatus::Running))
            {
                return;
            }

            onRender(inFrame, inData);
        }

        void Layer::cleanup()
        {
            if (!is(LayerStatus::Running))
            {
                return;
            }

            onCleanup();
        }

        void Layer::handle(const WindowEvent& inEvent)
        {
            onEvent(inEvent);
        }

        const String& Layer::getId() const
        {
            return m_id;
        }

        bool Layer::is(LayerStatus inStatus) const
        {
            return m_status == inStatus;
        }

        void Layer::setStatus(LayerStatus inStatus)
        {
            m_status = inStatus;
        }

        Viewport Layer::getViewport() const
        {
            const Vec<2, std::uint32_t> backendResolution = m_backend->getResolution();

            Viewport result = m_viewport;
            result.size.x   = result.size.x <= 0.0f ? backendResolution.x : result.size.x;
            result.size.y   = result.size.y <= 0.0f ? backendResolution.y : result.size.y;

            return result;
        }

        void Layer::setViewport(const Viewport& inValue)
        {
            m_viewport = inValue;
        }

        void Layer::setBackend(Backend* inBackend)
        {
            m_backend = inBackend;
        }
    }
}