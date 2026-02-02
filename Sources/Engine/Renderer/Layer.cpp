#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Layer::Layer(const Id& inId)
            : m_id(inId),
              m_status(LayerStatus::Offline),
              m_backend(nullptr)
        {}

        Layer::Layer()
            : m_id("Undefined"),
              m_status(LayerStatus::Offline),
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

        void Layer::resize(const Viewport& inViewport)
        {
            if (is(LayerStatus::Offline))
            {
                return;
            }

            onResize(inViewport);
        }

        void Layer::load(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (is(LayerStatus::Offline))
            {
                return;
            }

            onLoad(inType, inResource);
        }

        void Layer::load(const DrawTexture::List& inResources)
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

        void Layer::setup(const Frame& inFrame)
        {
            if (!is(LayerStatus::Running))
            {
                return;
            }

            if (!onSetup(inFrame))
            {
                return;
            }

            setStatus(LayerStatus::Running);
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

        bool Layer::is(LayerStatus inStatus) const
        {
            return m_status == inStatus;
        }

        void Layer::setStatus(LayerStatus inStatus)
        {
            m_status = inStatus;
        }

        const String& Layer::getId() const
        {
            return m_id;
        }
    }
}
