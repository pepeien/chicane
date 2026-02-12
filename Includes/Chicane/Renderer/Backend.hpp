#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/Window.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"
#include "Chicane/Renderer/Draw/Poly/Resource.hpp"
#include "Chicane/Renderer/Draw/Sky.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Draw/Texture/Resource.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        template <typename F = Frame>
        class CHICANE_RENDERER Backend
        {
        public:
            inline Backend()
                : m_window(nullptr),
                  m_layers({})
            {}

            inline virtual ~Backend() = default;

        public:
            inline virtual void onInit() { return; }

            inline virtual void onResize(const Vec<2, std::uint32_t>& inResolution)
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    if (inResolution.x > 0 && inResolution.y > 0)
                    {
                        Viewport viewport = layer->getViewport();
                        viewport.size     = inResolution;

                        layer->setViewport(viewport);
                    }

                    layer->resize(inResolution);
                }
            }

            inline virtual void onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->load(inType, inResource);
                }
            }

            inline virtual void onLoad(const DrawTextureResource& inResources)
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->load(inResources);
                }
            }

            inline virtual void onLoad(const DrawSky& inResource)
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->load(inResource);
                }
            }

            inline virtual void onSetup() { return; }

            inline virtual void onRender(const Frame& inFrame) { return; }

            inline virtual void onCleanup() { return; }

            inline virtual void onHandle(const WindowEvent& inEvent)
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->handle(inEvent);
                }
            }

        public:
            // Window
            inline void setWindow(const Window* inWindow) { m_window = inWindow; }

            // Layer
            template <typename Target = Layer<F>>
            inline Target* getLayer()
            {
                auto found = m_layers.find([](Layer<F>* inLayer) { return typeid(*inLayer) == typeid(Target); });

                if (found == m_layers.end())
                {
                    return nullptr;
                }

                return static_cast<Target*>(*found);
            }

            template <typename Target = Layer<F>, typename... Params>
            inline void addLayer(const ListPush<Layer<F>*>& inSettings, Params... inParams)
            {
                Target* layer = new Target(inParams...);
                layer->setBackend(this);
                layer->init();

                m_layers.add(layer, inSettings);
            }

        protected:
            // Layer
            inline void renderLayers(const F& inFrame, void* inData = nullptr)
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    if (!layer->setup(inFrame))
                    {
                        continue;
                    }

                    layer->render(inFrame, inData);
                    layer->cleanup();
                }
            }

            inline void destroyLayers()
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->destroy();
                }
            }

            inline void rebuildLayers()
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->rebuild();
                }
            }

            inline void deleteLayers()
            {
                for (Layer<F>* layer : m_layers)
                {
                    delete layer;
                    layer = nullptr;
                }

                m_layers.clear();
            }

        protected:
            // Window
            const Window*   m_window;

            // Layer
            List<Layer<F>*> m_layers;
        };
    }
}