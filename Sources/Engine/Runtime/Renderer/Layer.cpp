#include "Chicane/Runtime/Renderer/Layer.hpp"

namespace Chicane
{
    RendererLayer::RendererLayer()
        : RendererLayer("Unamed")
    {}

    RendererLayer::RendererLayer(const String& inId)
        : m_id(inId),
          m_status(RendererLayerStatus::Offline)
    {}

    void RendererLayer::init()
    {
        if (!is(RendererLayerStatus::Offline))
        {
            return;
        }

        if (!onInit())
        {
            return;
        }

        setStatus(RendererLayerStatus::Initialized);
    }

    void RendererLayer::destroy()
    {
        if (is(RendererLayerStatus::Offline))
        {
            return;
        }
    
        if (!onDestroy())
        {
            return;
        }

        setStatus(RendererLayerStatus::Offline);
    }

    void RendererLayer::rebuild()
    {
        if (!is(RendererLayerStatus::Offline))
        {
            return;
        }

        if (!onRebuild())
        {
            return;
        }

        setStatus(RendererLayerStatus::Initialized);
    }

    void RendererLayer::setup()
    {
        if (is(RendererLayerStatus::Offline))
        {
            return;
        }

        if (!onSetup())
        {
            return;
        }

        setStatus(RendererLayerStatus::Running);
    }

    void RendererLayer::render(void* outData)
    {
        if (!is(RendererLayerStatus::Running))
        {
            return;
        }

        onRender(outData);
    }

    void RendererLayer::handle(const WindowEvent& inEvent)
    {
        onEvent(inEvent);
    }

    bool RendererLayer::is(RendererLayerStatus inStatus) const
    {
        return m_status == inStatus;
    }

    RendererLayerStatus RendererLayer::getStatus() const
    {
        return m_status;
    }

    const String& RendererLayer::getId() const
    { 
        return m_id;
    }

    void RendererLayer::setStatus(RendererLayerStatus inStatus)
    {
        m_status = inStatus;
    }
}
