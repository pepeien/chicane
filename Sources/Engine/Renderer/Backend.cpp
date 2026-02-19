#include "Chicane/Renderer/Backend.hpp"

#include "Chicane/Renderer/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Backend::Backend()
            : m_renderer(nullptr)
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
            return;
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

        const Instance* Backend::getRenderer() const
        {
            return m_renderer;
        }

        Viewport Backend::getLayerViewport(Layer* inLayer) const
        {
            const Vec<2, std::uint32_t>& resolution = m_renderer->getResolution();
            const ViewportSettings       viewport   = inLayer->m_viewport;

            Size size;
            size.setIsAsobute(true);
            size.setRoot(resolution);
            size.setParent(resolution);

            Viewport result;
            result.size.x     = size.parse(viewport.width, SizeDirection::Horizontal);
            result.size.y     = size.parse(viewport.height, SizeDirection::Vertical);
            result.position.x = size.parse(viewport.offsetX, SizeDirection::Horizontal);
            result.position.y = size.parse(viewport.offsetY, SizeDirection::Vertical);

            return result;
        }

        std::vector<Layer*> Backend::findLayers(std::function<bool(const Layer* inLayer)> inPredicate) const
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

        std::size_t Backend::getResourceSize(BackendResource inType)
        {
            switch (inType)
            {
            case BackendResource::SceneIndices:
                return sizeof(Vertex::Index);

            case BackendResource::SceneVertices:
                return sizeof(Vertex);

            case BackendResource::SceneInstances:
                return sizeof(DrawPoly3DInstance);

            case BackendResource::Scene:
                return getResourceSize(BackendResource::SceneIndices) +
                       getResourceSize(BackendResource::SceneVertices) +
                       getResourceSize(BackendResource::SceneInstances);

            case BackendResource::UIIndices:
                return sizeof(Vertex::Index);

            case BackendResource::UIVertices:
                return sizeof(Vertex);

            case BackendResource::UIInstances:
                return sizeof(DrawPoly2DInstance);

            case BackendResource::UI:
                return getResourceSize(BackendResource::UIIndices) + getResourceSize(BackendResource::UIVertices) +
                       getResourceSize(BackendResource::UIInstances);

            default:
                return 0U;
            }
        }

        std::size_t Backend::getResourceBudget(BackendResource inType)
        {
            if (m_resourceBudget.find(inType) == m_resourceBudget.end())
            {
                return 0U;
            }

            const std::size_t resourceSize = getResourceSize(inType);

            return resourceSize * getResourceBudgetCount(inType);
        }

        std::uint32_t Backend::getResourceBudgetCount(BackendResource inType)
        {
            if (m_resourceBudget.find(inType) == m_resourceBudget.end())
            {
                return 0U;
            }

            const std::size_t resourceSize = getResourceSize(inType);

            return m_resourceBudget.at(inType) / resourceSize;
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

        void Backend::setResourceBudget(std::size_t inBytes)
        {
            m_resourceBudget[BackendResource::Scene]          = inBytes * 0.4f;                                  // 40%
            m_resourceBudget[BackendResource::SceneIndices]   = m_resourceBudget[BackendResource::Scene] * 0.2f; // 20%
            m_resourceBudget[BackendResource::SceneVertices]  = m_resourceBudget[BackendResource::Scene] * 0.6f; // 60%
            m_resourceBudget[BackendResource::SceneInstances] = m_resourceBudget[BackendResource::Scene] * 0.2f; // 20%

            m_resourceBudget[BackendResource::Texture] = inBytes * 0.4f; // 30%

            m_resourceBudget[BackendResource::UI]          = inBytes * 0.1f;                               // 10%
            m_resourceBudget[BackendResource::UIIndices]   = m_resourceBudget[BackendResource::UI] * 0.2f; // 20%
            m_resourceBudget[BackendResource::UIVertices]  = m_resourceBudget[BackendResource::UI] * 0.6f; // 60%
            m_resourceBudget[BackendResource::UIInstances] = m_resourceBudget[BackendResource::UI] * 0.2f; // 20%

            m_resourceBudget[BackendResource::Misc] = inBytes * 0.15f; // 15%
        }

        void Backend::setRenderer(const Instance* inValue)
        {
            m_renderer = inValue;
        }
    }
}