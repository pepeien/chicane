#pragma once

#include "Chicane/Core/List.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
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
            virtual void onInit() { return; }
            virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) { return; }
            virtual void onLoad(const DrawTexture::List& inResources) { return; }
            virtual void onSetup(const Frame& inFrame) { return; }
            virtual void onRender(const Frame& inFrame) { return; }
            virtual void onCleanup() { return; }

            virtual void onHandle(const WindowEvent& inEvent) {}

        public:
            void init();
            void load(DrawPolyType inType, const DrawPolyResource& inResource);
            void load(const DrawTexture::List& inResources);
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