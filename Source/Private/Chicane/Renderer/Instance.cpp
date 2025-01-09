#include "Chicane/Renderer/Instance.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

namespace Chicane
{
    namespace Renderer
    {
        Instance::Instance(Window::Instance* inWindow)
            : m_window(inWindow),
            m_layers({})
        {}

        bool Instance::hasLayer(Layer::Instance* inLayer)
        {
            if (!inLayer)
            {
                return false;
            }

            return hasLayer(inLayer->getId());
        }

        bool Instance::hasLayer(const std::string& inId)
        {
            return std::find_if(
                m_layers.begin(),
                m_layers.end(),
                [inId](Layer::Instance* _) { return _->getId() == inId; }
            ) != m_layers.end();
        }

        void Instance::pushLayerStart(Layer::Instance* inLayer)
        {
            if (hasLayer(inLayer))
            {
                return;
            }

            m_layers.insert(
                m_layers.begin(),
                inLayer
            );

            inLayer->build();
        }

        void Instance::pushLayer(
            Layer::Instance* inLayer,
            Layer::PushStrategy inPushStrategy,
            const std::string& inId
        )
        {
            if (!inLayer)
            {
                return;
            }

            switch (inPushStrategy)
            {
            case Layer::PushStrategy::Front:
                pushLayerStart(inLayer);

                return;

            case Layer::PushStrategy::BeforeLayer:
                pushLayerBefore(
                    inId,
                    inLayer
                );

                return;

            case Layer::PushStrategy::AfterLayer:
                pushLayerAfter(
                    inId,
                    inLayer
                );

                return;

            default:
                pushLayerBack(inLayer);

                return;
            }
        }

        void Instance::pushLayerBack(Layer::Instance* inLayer)
        {
            if (hasLayer(inLayer))
            {
                return;
            }

            m_layers.push_back(inLayer);

            inLayer->build();
        }

        void Instance::pushLayerBefore(const std::string& inId, Layer::Instance* inLayer)
        {
            if (hasLayer(inLayer))
            {
                return;
            }

            auto foundLayer = std::find_if(
                m_layers.begin(),
                m_layers.end(),
                [inId](Layer::Instance* _) { return _->getId() == inId; }
            );

            if (foundLayer == m_layers.end())
            {
                return;
            }

            m_layers.insert(
                foundLayer,
                inLayer
            );

            inLayer->build();
        }

        void Instance::pushLayerAfter(const std::string& inId, Layer::Instance* inLayer)
        {
            if (hasLayer(inLayer))
            {
                return;
            }

            auto foundLayer = std::find_if(
                m_layers.begin(),
                m_layers.end(),
                [inId](Layer::Instance* _) { return _->getId() == inId; }
            );

            if (foundLayer == m_layers.end())
            {
                return;
            }

            m_layers.insert(
                foundLayer + 1,
                inLayer
            );

            inLayer->build();
        }

        void Instance::emmitEventToLayers(const SDL_Event& inEvent)
        {
            for (Layer::Instance* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->onEvent(inEvent);
            }
        }

        void Instance::destroyLayers()
        {
            for (Layer::Instance* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->destroy();
            }
        }

        void Instance::rebuildLayers()
        {
            for (Layer::Instance* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->rebuild();
            }
        }

        void Instance::deleteLayers()
        {
            for (Layer::Instance* layer : m_layers)
            {
                delete layer;
            }

            m_layers.clear();
        }
    }
}