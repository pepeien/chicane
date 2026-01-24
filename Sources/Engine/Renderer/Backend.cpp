#include "Chicane/Renderer/Backend.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Backend::Backend(const Instance* inRenderer)
            : m_renderer(inRenderer),
              m_layers({})
        {}

        void Backend::onInit()
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->init();
            }
        }

        void Backend::onResize(const Viewport& inViewport)
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->resize(inViewport);
            }
        }

        void Backend::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->load(inType, inResource);
            }
        }

        void Backend::onLoad(const DrawTexture::List& inResources)
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->load(inResources);
            }
        }

        void Backend::onLoad(const DrawSky& inResource)
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->load(inResource);
            }
        }

        void Backend::onSetup(const Frame& inFrame)
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->setup(inFrame);
            }
        }

        void Backend::onRender(const Frame& inFrame)
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->render(inFrame);
            }
        }

        void Backend::onHandle(const WindowEvent& inEvent)
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->handle(inEvent);
            }
        }

        void Backend::onCleanup()
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->cleanup();
            }
        }

        void Backend::deleteLayers()
        {
            for (Layer* layer : m_layers)
            {
                delete layer;
                layer = nullptr;
            }

            m_layers.clear();
        }

        void Backend::destroyLayers()
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->destroy();
            }
        }

        void Backend::rebuildLayers()
        {
            for (Layer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->rebuild();
            }
        }
    }
}