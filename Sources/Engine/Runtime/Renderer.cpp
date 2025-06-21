#include "Chicane/Runtime/Renderer.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

namespace Chicane
{
    Renderer::Renderer()
        : m_size(Vec2::Zero),
        m_position(Vec2::Zero),
        m_layers({}),
        m_cameras({}),
        m_lights({}),
        m_meshes({})
    {
        loadEvents();
    }

    bool Renderer::canRender() const
    {
        for (RendererLayer* layer : m_layers)
        {
            if (!layer->is(RendererLayerStatus::Running))
            {
                continue;
            }

            return true;
        }

        return false;
    }

    void Renderer::init(const RendererCreateInfo& inCreateInfo)
    {
        setSize(inCreateInfo.resolution);
        setPosition(inCreateInfo.position);

        onInit();
    }

    void Renderer::render()
    {
        setupLayers();

        if (!canRender())
        {
            return;
        }

        onRender();
    }

    const Vec2& Renderer::getSize() const
    {
        return m_size;
    }

    void Renderer::setSize(const Vec2& inValue)
    {
        setSize(inValue.x, inValue.y);
    }

    void Renderer::setSize(float inWidth, float inHeight)
    {
        if (
            std::fabs(m_size.x - inWidth) < FLT_EPSILON &&
            std::fabs(m_size.y - inHeight) < FLT_EPSILON
        )
        {
            return;
        }

        m_size.x = inWidth;
        m_size.y = inHeight;

        onResizing();

        updateViewComponents();
    }

    const Vec2& Renderer::getPosition() const
    {
        return m_position;
    }

    void Renderer::setPosition(const Vec2& inValue)
    {
        setPosition(inValue.x, inValue.y);
    }

    void Renderer::setPosition(float inX, float inY)
    {
        if (
            std::fabs(m_position.x - inX) < FLT_EPSILON &&
            std::fabs(m_position.y - inY) < FLT_EPSILON
        )
        {
            return;
        }

        m_position.x = inX;
        m_position.y = inY;

        onRepositioning();
    }

    bool Renderer::hasLayer(RendererLayer* inLayer) const
    {
        if (!inLayer)
        {
            return false;
        }

        return hasLayer(inLayer->getId());
    }

    bool Renderer::hasLayer(const String& inId) const
    {
        return std::find_if(
            m_layers.begin(),
            m_layers.end(),
            [inId](RendererLayer* _) { return _->getId() == inId; }
        ) != m_layers.end();
    }

    void Renderer::pushLayerStart(RendererLayer* inLayer)
    {
        if (hasLayer(inLayer))
        {
            return;
        }

        m_layers.insert(
            m_layers.begin(),
            inLayer
        );

        inLayer->init();
    }

    void Renderer::pushLayer(
        RendererLayer* inLayer,
        RendererLayerPushStrategy inPushStrategy,
        const String& inId
    )
    {
        if (!inLayer)
        {
            return;
        }

        switch (inPushStrategy)
        {
        case RendererLayerPushStrategy::Front:
            pushLayerStart(inLayer);

            return;

        case RendererLayerPushStrategy::BeforeLayer:
            pushLayerBefore(
                inId,
                inLayer
            );

            return;

        case RendererLayerPushStrategy::AfterLayer:
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

    void Renderer::pushLayerBack(RendererLayer* inLayer)
    {
        if (hasLayer(inLayer))
        {
            return;
        }

        m_layers.push_back(inLayer);

        inLayer->init();
    }

    void Renderer::pushLayerBefore(const String& inId, RendererLayer* inLayer)
    {
        if (hasLayer(inLayer))
        {
            return;
        }

        auto foundLayer = std::find_if(
            m_layers.begin(),
            m_layers.end(),
            [inId](RendererLayer* _) { return _->getId() == inId; }
        );

        if (foundLayer == m_layers.end())
        {
            return;
        }

        m_layers.insert(
            foundLayer,
            inLayer
        );

        inLayer->init();
    }

    void Renderer::pushLayerAfter(const String& inId, RendererLayer* inLayer)
    {
        if (hasLayer(inLayer))
        {
            return;
        }

        auto foundLayer = std::find_if(
            m_layers.begin(),
            m_layers.end(),
            [inId](RendererLayer* _) { return _->getId() == inId; }
        );

        if (foundLayer == m_layers.end())
        {
            return;
        }

        m_layers.insert(foundLayer + 1, inLayer);

        inLayer->init();
    }

    void Renderer::setupLayers()
    {
        for (RendererLayer* layer : m_layers)
        {
            layer->setup();
        }
    }

    void Renderer::destroyLayers()
    {
        for (RendererLayer* layer : m_layers)
        {
            if (!layer)
            {
                continue;
            }

            layer->destroy();
        }
    }

    void Renderer::rebuildLayers()
    {
        for (RendererLayer* layer : m_layers)
        {
            if (!layer)
            {
                continue;
            }

            layer->rebuild();
        }
    }

    void Renderer::deleteLayers()
    {
        for (RendererLayer* layer : m_layers)
        {
            delete layer;
            layer = nullptr;
        }

        m_layers.clear();
    }

    void Renderer::loadEvents()
    {
        Application::getWindow()->watchEvent(
            [this](WindowEvent inEvent)
            {
                onEvent(inEvent);

                for (RendererLayer* layer : m_layers)
                {
                    if (!layer)
                    {
                        continue;
                    }

                    layer->handle(inEvent);
                }
            }
        );

        Application::watchScene(
            [this](Scene* inLevel) {
                if (!inLevel)
                {
                    return;
                }

                inLevel->watchComponents(
                    [this](const std::vector<Component*>& inComponents) {
                        m_cameras.clear();
                        m_lights.clear();
                        m_meshes.clear();

                        for (Component* component : inComponents)
                        {
                            if (component->isType<CCamera>())
                            {
                                m_cameras.push_back(static_cast<CCamera*>(component));
                            }

                            if (component->isType<CLight>())
                            {
                                m_lights.push_back(static_cast<CLight*>(component));
                            }

                            if (component->isType<CMesh>())
                            {
                                m_meshes.push_back(static_cast<CMesh*>(component));
                            }
                        }

                        std::sort(
                            m_meshes.begin(),
                            m_meshes.end(),
                            [](CMesh* inA, CMesh* inB)
                            {
                                return inA->getModel().compare(inB->getModel()) > 0;
                            }
                        );

                        updateViewComponents();
                    }
                );
            }
        );
    }

    void Renderer::updateViewComponents()
    {
        for (CCamera* camera : m_cameras)
        {
            camera->setViewport(m_size);
        }

        for (CLight* light : m_lights)
        {
            light->setViewport(m_size);
        }
    }
}