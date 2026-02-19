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

        std::size_t Backend::getResourceSize(Resource inType)
        {
            switch (inType)
            {
            case Resource::SceneIndices:
                return sizeof(Vertex::Index);

            case Resource::SceneVertices:
                return sizeof(Vertex);

            case Resource::SceneInstances:
                return sizeof(DrawPoly3DInstance);

            case Resource::Scene:
                return getResourceSize(Resource::SceneIndices) + getResourceSize(Resource::SceneVertices) +
                       getResourceSize(Resource::SceneInstances);

            case Resource::UIIndices:
                return sizeof(Vertex::Index);

            case Resource::UIVertices:
                return sizeof(Vertex);

            case Resource::UIInstances:
                return sizeof(DrawPoly2DInstance);

            case Resource::UI:
                return getResourceSize(Resource::UIIndices) + getResourceSize(Resource::UIVertices) +
                       getResourceSize(Resource::UIInstances);

            default:
                return 0U;
            }
        }

        std::size_t Backend::getResourceBudget(Resource inType)
        {
            const ResourceBudget& budget = m_renderer->getResourceBudget();

            if (budget.find(inType) == budget.end())
            {
                return 0U;
            }

            const std::size_t resourceSize = getResourceSize(inType);

            return resourceSize * getResourceBudgetCount(inType);
        }

        std::uint32_t Backend::getResourceBudgetCount(Resource inType)
        {
            const ResourceBudget& budget = m_renderer->getResourceBudget();

            if (budget.find(inType) == budget.end())
            {
                return 0U;
            }

            const std::size_t resourceSize = getResourceSize(inType);

            return (budget.at(inType) * m_VRAM) / resourceSize;
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

        void Backend::setVRAM(std::size_t inBytes)
        {
            m_VRAM = inBytes;
        }

        void Backend::setRenderer(const Instance* inValue)
        {
            m_renderer = inValue;
        }
    }
}