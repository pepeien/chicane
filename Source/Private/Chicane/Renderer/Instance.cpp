#include "Chicane/Renderer/Instance.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

namespace Chicane
{
    Renderer::Renderer(Window* inWindow)
        : m_window(inWindow),
        m_layers({})
    {}

    void Renderer::onEvent(const SDL_Event& inEvent)
    {
        for (Layer* layer : m_layers)
        {
            layer->onEvent(inEvent);
        }
    }

    bool Renderer::hasLayer(Layer* inLayer)
    {
        if (inLayer == nullptr)
        {
            return false;
        }

        return hasLayer(inLayer->getId());
    }

    bool Renderer::hasLayer(const std::string& inId)
    {
        return std::find_if(
            m_layers.begin(),
            m_layers.end(),
            [inId](Layer* _) { return _->getId() == inId; }
        ) != m_layers.end();
    }

    void Renderer::pushLayerStart(Layer* inLayer)
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

    void Renderer::pushLayer(
        Layer* inLayer,
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

    void Renderer::pushLayerBack(Layer* inLayer)
    {
        if (hasLayer(inLayer))
        {
            return;
        }

        m_layers.push_back(inLayer);

        inLayer->build();
    }

    void Renderer::pushLayerBefore(const std::string& inId, Layer* inLayer)
    {
        if (hasLayer(inLayer))
        {
            return;
        }

        auto foundLayer = std::find_if(
            m_layers.begin(),
            m_layers.end(),
            [inId](Layer* _) { return _->getId() == inId; }
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

    void Renderer::pushLayerAfter(const std::string& inId, Layer* inLayer)
    {
        if (hasLayer(inLayer))
        {
            return;
        }

        auto foundLayer = std::find_if(
            m_layers.begin(),
            m_layers.end(),
            [inId](Layer* _) { return _->getId() == inId; }
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

    void Renderer::destroyLayers()
    {
        for (Layer* layer : m_layers)
        {
            layer->destroy();
        }
    }

    void Renderer::rebuildLayers()
    {
        for (Layer* layer : m_layers)
        {
            layer->rebuild();
        }
    }

    void Renderer::deleteLayers()
    {
        for (Layer* layer : m_layers)
        {
            delete layer;
        }

        m_layers.clear();
    }
}