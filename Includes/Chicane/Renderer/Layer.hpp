#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer/Status.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER Layer
        {
        public:
            Layer(const String& inId);
            Layer();

            virtual ~Layer();

        protected:
            virtual bool onInit() { return true; }
            virtual bool onDestroy() { return true; }
            virtual bool onRebuild() { return true; }
            virtual bool onSetup() { return true; }
            virtual void onRender(void* outData) { return; }
            virtual bool onCleanup() { return true; }
            virtual void onEvent(const WindowEvent& inEvent) { return; }

        public:
            void init();
            void destroy();
            void rebuild();
            void setup();
            void render(void* outData);
            void cleanup();
            void handle(const WindowEvent& inEvent);

        public:
            bool is(LayerStatus inStatus) const;

            const String& getId() const;

        protected:
            void setStatus(LayerStatus inStatus);

            template <typename T>
            bool hasLayer() const
            {
                for (const Layer* layer : m_children)
                {
                    if (typeid(T) != typeid(*layer))
                    {
                        continue;
                    }

                    return true;
                }

                return false;
            }

            template <typename Target, typename Anchor = Layer, typename... Params>
            void pushLayer(ListPushStrategy inStrategy = ListPushStrategy::Back, Params... inParams)
            {
                m_children.add(new Target(inParams...), inStrategy);
            }

        protected:
            String       m_id;
            LayerStatus  m_status;
            List<Layer*> m_children;
        };
    }
}