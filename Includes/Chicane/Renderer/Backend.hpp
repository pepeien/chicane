#pragma once

#include "Chicane/Core/List.hpp"
#include "Chicane/Core/Window.hpp"

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
        template <typename F = Frame>
        class CHICANE_RENDERER Backend
        {
        public:
            Backend(const Window* inWindow)
                : m_window(inWindow),
                  m_layers({})
            {}

            inline virtual ~Backend() = default;

        public:
            inline virtual void onInit()
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->init();
                }
            }

            inline virtual void onResize(const Viewport& inViewport)
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->resize(inViewport);
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

            inline virtual void onLoad(const DrawTexture::List& inResources)
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

            inline virtual void onSetup()
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->setup(getCurrentFrame());
                }
            }

            inline virtual void onRender()
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->render(getCurrentFrame());
                }

                m_currentFrame = (m_currentFrame + 1) % m_frames.size();
            }

            inline virtual void onCleanup()
            {
                for (Layer<F>* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->cleanup();
                }
            }

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
            // Frame
            inline F& getCurrentFrame() { return m_frames.at(m_currentFrame); }

            // Render
            inline void useCamera(const View& inData) { getCurrentFrame().useCamera(inData); }

            inline void addLight(const View& inData) { getCurrentFrame().addLight(inData); }

            inline void addLight(const View::List& inData) { getCurrentFrame().addLights(inData); }

            template <typename T>
            inline void drawPoly(Draw::Id inId, const T& inInstance)
            {
                getCurrentFrame().use(inId, inInstance);
            }

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
            // Frame
            inline void setFrameCount(std::uint32_t inValue) { m_frames.resize(inValue); }

            // Layer
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

            // Frame
            std::vector<F>  m_frames;
            std::uint32_t   m_currentFrame;
        };
    }
}