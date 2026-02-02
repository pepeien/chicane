#pragma once

#include "Chicane/Core/List.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
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
            virtual ~Backend() = default;

        public:
            virtual void onInit();
            virtual void onResize(const Viewport& inViewport);
            virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource);
            virtual void onLoad(const DrawTexture::List& inResources);
            virtual void onLoad(const DrawSky& inResource);
            virtual void onSetup(const Frame& inFrame);
            virtual void onRender(const Frame& inFrame);
            virtual void onCleanup();

            virtual void onHandle(const WindowEvent& inEvent);

        public:
            // Layer
            template <typename Target = Layer>
            Target* getLayer()
            {
                auto found = m_layers.find([](Layer* inLayer) { return typeid(*inLayer) == typeid(Target); });

                if (found == m_layers.end())
                {
                    return nullptr;
                }

                return static_cast<Target*>(*found);
            }

            template <typename Target = Layer, typename... Params>
            void addLayer(const ListPush<Layer*>& inSettings, Params... inParams)
            {
                Target* layer = new Target(inParams...);
                layer->setBackend(this);
                layer->init();

                m_layers.add(layer, inSettings);
            }

        protected:
            void deleteLayers();
            void destroyLayers();
            void rebuildLayers();

        protected:
            const Instance* m_renderer;

            List<Layer*>    m_layers;
        };
    }
}