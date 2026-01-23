#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Layer/Status.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class Backend;

        class CHICANE_RENDERER Layer
        {
        public:
            using Id = String;

        public:
            Layer(const Id& inId);
            Layer();

            virtual ~Layer() = default;

        protected:
            virtual bool onInit() { return true; }
            virtual bool onDestroy() { return true; }
            virtual bool onRebuild() { return true; }
            virtual void onResize(const Viewport& inViewport) { return; }
            virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) { return; }
            virtual void onLoad(const DrawTexture::List& inResources) { return; }
            virtual void onLoad(const DrawSky& inResource) { return; }
            virtual bool onSetup(const Frame& inFrame) { return true; }
            virtual void onRender(const Frame& inFrame, void* inData = nullptr) { return; }
            virtual void onCleanup() { return; }

            virtual void onEvent(const WindowEvent& inEvent) { return; }

        public:
            void init();
            void destroy();
            void rebuild();
            void resize(const Viewport& inViewport);
            void load(DrawPolyType inType, const DrawPolyResource& inResource);
            void load(const DrawTexture::List& inResources);
            void load(const DrawSky& inResource);
            void setup(const Frame& inFrame);
            void render(const Frame& inFrame, void* inData = nullptr);
            void cleanup();

            void handle(const WindowEvent& inEvent);

        public:
            bool is(LayerStatus inStatus) const;
            void setStatus(LayerStatus inStatus);

            template <typename T = Backend>
            T* getBackend()
            {
                return static_cast<T*>(m_backend);
            }
            void setBackend(Backend* inBackend) { m_backend = inBackend; }

            template <typename Target = Layer, typename Anchor = Layer, typename... Params>
            void addLayer(ListPushStrategy inStrategy = ListPushStrategy::Back, Params... inParams)
            {
                Target* layer = new Target(inParams...);
                layer->setBackend(m_backend);
                layer->setParent(this);
                layer->init();

                m_children.add(layer, inStrategy);
            }

            template <typename T = Layer>
            T* getParent()
            {
                return static_cast<T*>(m_parent);
            }
            void setParent(Layer* inLayer) { m_parent = inLayer; }

        protected:
            void deleteChildren();

        protected:
            Id           m_id;
            LayerStatus  m_status;

            Backend*     m_backend;

            Layer*       m_parent;
            List<Layer*> m_children;
        };
    }
}