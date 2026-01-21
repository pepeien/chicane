#pragma once

#include "Chicane/Core/List.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class Instance;

        class CHICANE_RENDERER Backend
        {
        public:
            Backend(const Instance* inRenderer);
            virtual ~Backend();

        protected:
            virtual void onInit() {}
            virtual void onSetup(const Frame& inFrame) {}
            virtual void onRender(const Frame& inFrame) {}
            virtual void onCleanup() {}

            virtual void onHandle(const WindowEvent& inEvent) {}

        public:
            void init();
            void setup(const Frame& inFrame);
            void render(const Frame& inFrame);
            void handle(const WindowEvent& inEvent);
            void cleanup();

        public:
            // Layer
            template <typename Target, typename Anchor = Layer, typename... Params>
            void addLayer(ListPushStrategy inStrategy = ListPushStrategy::Back, Params... inParams)
            {
                m_layers.add(new Target(inParams...), inStrategy);
            }

        protected:
            const Instance* m_renderer;

            List<Layer*>    m_layers;
        };
    }
}