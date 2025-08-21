#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Renderer/Layer/Status.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME RendererLayer
    {
    protected:
        using Super = RendererLayer;

    public:
        RendererLayer(const String& inId);

        virtual ~RendererLayer();

    protected:
        virtual bool onInit() { return true; }
        virtual bool onDestroy() { return true; }
        virtual bool onRebuild() { return true; }
        virtual bool onSetup() { return true; }
        virtual void onRender(void* outData) { return; }
        virtual void onEvent(const WindowEvent& inEvent) { return; }

    public:  
        void init();
        void destroy();
        void rebuild();
        void setup();
        void render(void* outData);
        void handle(const WindowEvent& inEvent);

    public:
        bool is(RendererLayerStatus inStatus) const;
        RendererLayerStatus getStatus() const;

        const String& getId() const;

    protected:
        void setStatus(RendererLayerStatus inStatus);

    protected:
        String                      m_id;
        RendererLayerStatus         m_status;
        std::vector<RendererLayer*> m_children;
    };
}