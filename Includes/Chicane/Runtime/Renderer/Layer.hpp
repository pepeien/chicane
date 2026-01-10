#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Layer/Status.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME RendererLayer
    {
    protected:
        using Super = RendererLayer;

    public:
        RendererLayer(const String& inId);
        RendererLayer();

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

        template <typename T>
        bool hasLayer() const
        {
            for (const RendererLayer* layer : m_children)
            {
                if (typeid(T) != typeid(*layer))
                {
                    continue;
                }

                return true;
            }

            return false;
        }

        template <typename Target, typename Anchor = RendererLayer, typename... Params>
        void pushLayer(ListPushStrategy inStrategy = ListPushStrategy::Back, Params... inParams)
        {
            m_children.add(new Target(inParams...), inStrategy);
        }

    protected:
        String               m_id;
        RendererLayerStatus  m_status;
        List<RendererLayer*> m_children;
    };
}