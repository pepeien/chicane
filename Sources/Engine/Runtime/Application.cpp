#include "Chicane/Runtime/Application.hpp"

#include <iostream>
#include <thread>

#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Model/Manager.hpp"
#include "Chicane/Box/Texture/Manager.hpp"

#include "Chicane/Kerb.hpp"

#include "Chicane/Runtime/Scene/Actor/Sky.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

namespace Chicane
{
    Application::Application()
        : m_telemetry({}),
          m_controller(nullptr),
          m_controllerObservable({}),
          m_scene(nullptr),
          m_sceneObservable({}),
          m_view(nullptr),
          m_viewObservable({}),
          m_window(nullptr),
          m_renderer(nullptr)
    {}

    Application::~Application()
    {}

    void Application::run(const ApplicationCreateInfo& inCreateInfo)
    {
        initWindow(inCreateInfo.window);
        initRenderer(inCreateInfo.window.backend, inCreateInfo.renderer);
        initBox();
        initKerb();

        if (inCreateInfo.onSetup)
        {
            inCreateInfo.onSetup();
        }

        while (m_window->run())
        {
            m_telemetry.start();

            render();

            m_telemetry.end();
        }
    }

    void Application::render()
    {
        renderScene();
        renderView();

        m_renderer->render();
    }

    const Telemetry& Application::getTelemetry() const
    {
        return m_telemetry;
    }

    bool Application::hasController()
    {
        return m_controller != nullptr;
    }

    Controller* Application::getController()
    {
        return m_controller;
    }

    void Application::setController(Controller* inController)
    {
        if (inController == m_controller)
        {
            return;
        }

        m_controller = inController;

        m_controllerObservable.next(m_controller);
    }

    Application::ControllerSubscription Application::watchController(
        ControllerSubscription::NextCallback     inNext,
        ControllerSubscription::ErrorCallback    inError,
        ControllerSubscription::CompleteCallback inComplete
    )
    {
        return m_controllerObservable.subscribe(inNext, inError, inComplete).next(m_controller);
    }

    bool Application::hasScene()
    {
        return m_scene && m_scene.get() != nullptr;
    }

    Application::SceneSubscription Application::watchScene(
        SceneSubscription::NextCallback     inNext,
        SceneSubscription::ErrorCallback    inError,
        SceneSubscription::CompleteCallback inComplete
    )
    {
        return m_sceneObservable.subscribe(inNext, inError, inComplete).next(m_scene.get());
    }

    bool Application::hasView()
    {
        return m_view != nullptr;
    }

    Application::ViewSubscription Application::watchView(
        ViewSubscription::NextCallback     inNext,
        ViewSubscription::ErrorCallback    inError,
        ViewSubscription::CompleteCallback inComplete
    )
    {
        return m_viewObservable.subscribe(inNext, inError, inComplete).next(m_view.get());
    }

    bool Application::hasWindow()
    {
        return m_window && m_window.get() != nullptr;
    }

    Window* Application::getWindow()
    {
        if (!hasWindow())
        {
            return nullptr;
        }

        return m_window.get();
    }

    bool Application::hasRenderer()
    {
        return m_renderer && m_renderer.get() != nullptr;
    }

    Renderer::Instance* Application::getRenderer()
    {
        if (!hasRenderer())
        {
            return nullptr;
        }

        return m_renderer.get();
    }

    void Application::setRenderer(WindowBackend inBackend)
    {
        if (!hasWindow() || !hasRenderer())
        {
            return;
        }

        m_renderer->destroy();

        m_window->setBackend(inBackend);
        m_renderer->setBackend(inBackend);
    }

    void Application::initWindow(const WindowSettings& inSettings)
    {
        if (hasWindow())
        {
            return;
        }

        m_window = std::make_unique<Window>();
        m_window->init(inSettings);
        m_window->watchEvent(
            [&](WindowEvent inEvent)
            {
                if (hasView())
                {
                    m_view->handle(inEvent);
                }
            }
        );
    }

    void Application::initRenderer(WindowBackend inBackend, const Renderer::Settings& inSettings)
    {
        if (hasRenderer())
        {
            return;
        }

        m_renderer = std::make_unique<Renderer::Instance>();
        m_renderer->init(getWindow(), inBackend, inSettings);
        m_renderer->watchViewport(
            [&](const Renderer::Viewport& inViewport)
            {
                if (hasView())
                {
                    m_view->setSize(inViewport.size.x, inViewport.size.y);
                }
            }
        );
    }

    void Application::initBox()
    {
        Box::getModelManager()->watch(
            [&](const Box::ModelManager::Instances& inInstances)
            {
                for (const auto& [id, poly] : inInstances)
                {
                    Renderer::DrawPolyData data;
                    data.reference = id;
                    data.vertices  = poly.vertices;
                    data.indices   = poly.indices;

                    m_renderer->loadPoly(Renderer::DrawPolyType::e3D, data);
                }
            }
        );
        Box::getTextureManager()->watch(
            [&](const Box::TextureManager::Instances& inInstances)
            {
                for (const auto& [id, texture] : inInstances)
                {
                    Renderer::DrawTextureData data;
                    data.reference = id;
                    data.image     = texture;

                    m_renderer->loadTexture(data);
                }
            }
        );

        Box::init();
    }

    void Application::initKerb()
    {
        Kerb::init();
    }

    void Application::renderScene()
    {
        if (!hasScene())
        {
            return;
        }

        m_scene->tick(m_telemetry.delta);

        for (CCamera* camera : m_scene->getActiveComponents<CCamera>())
        {
            camera->setViewport(m_renderer->getViewport().size);

            m_renderer->useCamera(camera->getData());
        }

        for (CLight* light : m_scene->getActiveComponents<CLight>())
        {
            m_renderer->addLight(light->getData());
        }

        for (CMesh* mesh : m_scene->getActiveComponents<CMesh>())
        {
            if (!mesh->hasMesh())
            {
                continue;
            }

            Renderer::DrawPoly3DInstance draw;
            draw.model = mesh->getTransform().getMatrix();

            for (const Box::MeshGroup& group : mesh->getMesh()->getGroups())
            {
                draw.texture = m_renderer->findTexture(group.getTexture());

                m_renderer->drawPoly(Renderer::DrawPolyType::e3D, group.getModel(), draw);
            }
        }

        for (ASky* sky : m_scene->getActors<ASky>())
        {
            const Box::Sky* asset = sky->getSky();

            Renderer::DrawSkyData data;
            data.reference = asset->getFilepath().string();
            data.model     = asset->getModel();

            for (Box::SkySide side : Box::Sky::ORDER)
            {
                data.textures.push_back(asset->getSide(side));
            }

            m_renderer->loadSky(data);
        }
    }

    void Application::renderView()
    {
        if (!hasView())
        {
            return;
        }

        m_view->tick(m_telemetry.delta);

        const Vec2& viewSize = m_view->getSize();

        for (Grid::Component* component : m_view->getChildrenFlat())
        {
            if (!component->isDrawable())
            {
                continue;
            }

            const Grid::Primitive& primitive = component->getPrimitive();

            Renderer::DrawPolyData data;
            data.vertices         = primitive.vertices;
            data.indices          = primitive.indices;
            Renderer::Draw::Id id = m_renderer->loadPoly(Renderer::DrawPolyType::e2D, data);

            const Vec2&        position = component->getPosition();
            const Grid::Style& style    = component->getStyle();

            const Color::Rgba backgroundColor = style.background.color.get();

            Renderer::DrawPoly2DInstance draw;
            draw.screen   = viewSize;
            draw.size     = component->getSize();
            draw.position = {position.x, position.y, style.zIndex.get()};
            draw.texture  = m_renderer->findTexture(style.background.image.get());
            draw.color    = {
                backgroundColor.r,
                backgroundColor.g,
                backgroundColor.b,
                (draw.texture >= 0 ? 255.0f : backgroundColor.a) * style.opacity.get()
            };
            m_renderer->drawPoly(id, draw);
        }
    }
}