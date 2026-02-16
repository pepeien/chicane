#include "Chicane/Renderer/Backend.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Backend::Backend()
            : m_window(nullptr)
        {}

        void Backend::onInit()
        {
            return;
        }

        void Backend::onShutdown()
        {
            return;
        }

        void Backend::onResize()
        {
            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onResize(m_resolution);
            }
        }

        void Backend::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onLoad(inType, inResource);
            }
        }

        void Backend::onLoad(const DrawTextureResource& inResources)
        {
            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onLoad(inResources);
            }
        }

        void Backend::onLoad(const DrawSky& inResource)
        {
            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onLoad(inResource);
            }
        }

        void Backend::onBeginRender()
        {
            return;
        }

        void Backend::onRender(const Frame& inFrame)
        {
            return;
        }

        void Backend::onEndRender()
        {
            return;
        }

        const Window* Backend::getWindow() const
        {
            return m_window;
        }

        void Backend::setWindow(const Window* inWindow)
        {
            m_window = inWindow;
        }

        const Vec<2, std::uint32_t>& Backend::getResolution() const
        {
            return m_resolution;
        }

        void Backend::setResolution(const Vec<2, std::uint32_t>& inValue)
        {
            m_resolution = inValue;
        }

        std::vector<Layer*> Backend::findLayers(std::function<bool(Layer* inLayer)> inPredicate) const
        {
            std::vector<Layer*> result;

            for (const std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!inPredicate(layer.get()))
                {
                    continue;
                }

                result.push_back(layer.get());
            }

            return result;
        }

        void Backend::renderLayers(const Frame& inFrame, void* inData)
        {
            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                if (!layer->onBeginRender(inFrame))
                {
                    continue;
                }

                layer->onRender(inFrame, inData);
                layer->onEndRender();
            }
        }

        void Backend::shutdownLayers()
        {
            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onShutdown();
            }
        }

        void Backend::rebuildLayers()
        {
            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onRestart();
            }
        }

        void Backend::destroyLayers()
        {
            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onDestruction();
            }

            m_layers.clear();
        }
    }
}