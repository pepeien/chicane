#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Resource.hpp"
#include "Chicane/Renderer/Frame.hpp"
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
            virtual bool onSetup(const Frame& inFrame) { return true; }
            virtual void onRender(const Frame& inFrame) { return; }
            virtual void onCleanup() { return; }

            virtual void onEvent(const WindowEvent& inEvent) { return; }

        public:
            void init();
            void destroy();
            void rebuild();
            void setup(const Frame& inFrame);
            void render(const Frame& inFrame);
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
            void addLayer(ListPushStrategy inStrategy = ListPushStrategy::Back, Params... inParams)
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