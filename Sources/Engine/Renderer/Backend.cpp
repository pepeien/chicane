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

        void Backend::onResize(const Vec<2, std::uint32_t>& inResolution)
        {
            m_resolution = inResolution;

            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->resize(inResolution);
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

                layer->load(inType, inResource);
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

                layer->load(inResources);
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

                layer->load(inResource);
            }
        }

        void Backend::onSetup()
        {
            return;
        }

        void Backend::onRender(const Frame& inFrame)
        {
            return;
        }

        void Backend::onCleanup()
        {
            return;
        }

        void Backend::onHandle(const WindowEvent& inEvent)
        {
            for (std::unique_ptr<Layer>& layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->handle(inEvent);
            }
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

                if (!layer->setup(inFrame))
                {
                    continue;
                }

                layer->render(inFrame, inData);
                layer->cleanup();
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

                layer->destroy();
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

                layer->rebuild();
            }
        }

        void Backend::deleteLayers()
        {
            m_layers.clear();
        }
    }
}