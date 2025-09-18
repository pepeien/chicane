#include "Chicane/Runtime/Renderer/Layer.hpp"

namespace Chicane
{
    RendererLayer::RendererLayer(const String& inId)
        : m_id(inId),
          m_status(RendererLayerStatus::Offline),
          m_children({})
    {}

    RendererLayer::RendererLayer()
        : m_id("Undefined"),
          m_status(RendererLayerStatus::Offline),
          m_children({})
    {}

    RendererLayer::~RendererLayer()
    {
        for (RendererLayer* child : m_children)
        {
            delete child;
            child = nullptr;
        }

        m_children.clear();
    }

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

        for (RendererLayer* child : m_children)
        {
            if (!child)
            {
                continue;
            }

            child->init();
        }
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

        for (RendererLayer* child : m_children)
        {
            if (!child)
            {
                continue;
            }

            child->destroy();
        }
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

        for (RendererLayer* child : m_children)
        {
            if (!child)
            {
                continue;
            }

            child->rebuild();
        }
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

        for (RendererLayer* child : m_children)
        {
            if (!child)
            {
                continue;
            }

            child->setup();
        }
    }

    void RendererLayer::render(void* outData)
    {
        if (!is(RendererLayerStatus::Running))
        {
            return;
        }

        onRender(outData);

        for (RendererLayer* child : m_children)
        {
            if (!child)
            {
                continue;
            }

            child->render(outData);
        }
    }

    void RendererLayer::handle(const WindowEvent& inEvent)
    {
        onEvent(inEvent);

        for (RendererLayer* child : m_children)
        {
            if (!child)
            {
                continue;
            }

            child->onEvent(inEvent);
        }
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
