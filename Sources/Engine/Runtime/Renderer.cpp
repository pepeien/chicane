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
          m_sizeOberservable({}),
          m_position(Vec2::Zero),
          m_positionOberservable({}),
          m_window(nullptr),
          m_layers({}),
          m_camera(nullptr),
          m_light(nullptr),
          m_meshes({})
    {
        loadEvents();
    }

    bool Renderer::canRender() const
    {
        for (const RendererLayer* layer : m_layers)
        {
            if (!layer)
            {
                continue;
            }

            if (!layer->is(RendererLayerStatus::Running))
            {
                continue;
            }

            return true;
        }

        return false;
    }

    void Renderer::init(Window* inWindow)
    {
        setSize(Vec2::Zero);
        setPosition(Vec2::Zero);
        setWindow(inWindow);

        onInit();
    }

    void Renderer::render()
    {
        setupLayers();
        setupComponents();

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
        if (std::fabs(m_size.x - inWidth) < FLT_EPSILON && std::fabs(m_size.y - inHeight) < FLT_EPSILON)
        {
            return;
        }

        m_size.x = inWidth;
        m_size.y = inHeight;

        onResizing();

        m_sizeOberservable.next(m_size);
    }

    Renderer::SizeSubscription Renderer::watchSize(
        SizeObservable::NextCallback     inNext,
        SizeObservable::ErrorCallback    inError,
        SizeObservable::CompleteCallback inComplete
    )
    {
        return m_sizeOberservable.subscribe(inNext, inError, inComplete).next(m_size);
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
        if (std::fabs(m_position.x - inX) < FLT_EPSILON && std::fabs(m_position.y - inY) < FLT_EPSILON)
        {
            return;
        }

        m_position.x = inX;
        m_position.y = inY;

        onRepositioning();

        m_positionOberservable.next(m_position);
    }

    Renderer::PositionSubscription Renderer::watchPosition(
        PositionObservable::NextCallback     inNext,
        PositionObservable::ErrorCallback    inError,
        PositionObservable::CompleteCallback inComplete
    )
    {
        return m_positionOberservable.subscribe(inNext, inError, inComplete).next(m_position);
    }

    Window* Renderer::getWindow() const
    {
        return m_window;
    }

    void Renderer::setWindow(Window* inWindow)
    {
        if (inWindow == m_window)
        {
            return;
        }

        m_window = inWindow;
    }

    const std::vector<CMesh*>& Renderer::getMeshes() const
    {
        return m_meshes;
    }

    void Renderer::setupLayers()
    {
        for (RendererLayer* layer : m_layers)
        {
            if (!layer)
            {
                continue;
            }

            layer->setup();
        }
    }

    void Renderer::cleanupLayers()
    {
        for (RendererLayer* layer : m_layers)
        {
            if (!layer)
            {
                continue;
            }

            layer->cleanup();
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

    void Renderer::setupComponents()
    {
        m_camera = nullptr;
        m_light  = nullptr;
        m_meshes.clear();

        for (Component* component : Application::getScene()->getComponents())
        {
            if (!component->isActive())
            {
                continue;
            }

            if (component->isType<CCamera>())
            {
                if (!m_camera)
                {
                    m_camera = static_cast<CCamera*>(component);
                }

                continue;
            }

            if (component->isType<CLight>())
            {
                if (!m_light)
                {
                    m_light = static_cast<CLight*>(component);
                }

                continue;
            }

            if (component->isType<CMesh>())
            {
                m_meshes.push_back(static_cast<CMesh*>(component));

                continue;
            }
        }

        std::sort(m_meshes.begin(), m_meshes.end(), [](CMesh* inA, CMesh* inB) {
            return inA->getModel().compare(inB->getModel()) > 0;
        });
    }

    void Renderer::loadEvents()
    {
        Application::getWindow()->watchEvent([this](WindowEvent inEvent) {
            onEvent(inEvent);

            for (RendererLayer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->handle(inEvent);
            }
        });
    }
}