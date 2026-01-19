#include "Chicane/Runtime/Application.hpp"

#include <iostream>
#include <thread>

#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Model/Manager.hpp"

#include "Chicane/Kerb.hpp"

#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

namespace Chicane
{
    Application::Application()
        : m_bIsRunning(true),
          m_telemetry({}),
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
        Box::init();
        Kerb::init();

        initWindow(inCreateInfo.window);
        initRenderer(inCreateInfo.window.renderer);

        if (inCreateInfo.onSetup)
        {
            inCreateInfo.onSetup();
        }

        while (m_window->run())
        {
            m_telemetry.start();

            m_scene->tick(m_telemetry.delta);
            m_view->tick(m_telemetry.delta);

            m_renderer->useCamera(m_scene->getActiveComponents<CCamera>().at(0)->getData());
            m_renderer->addLight(m_scene->getActiveComponents<CLight>().at(0)->getData());

            for (CMesh* mesh : m_scene->getActiveComponents<CMesh>())
            {
                if (!mesh->isDrawable())
                {
                    continue;
                }

                for (const Box::MeshGroup& group : mesh->getMesh()->getGroups())
                {
                    const Box::ModelExtracted& model = Box::getModelManager()->getInstance(group.getModel());

                    Renderer::DrawData3D draw;
                    draw.vertices = model.vertices;
                    draw.indices  = model.indices;
                    draw.model    = mesh->getTransform().getMatrix();

                    m_renderer->draw(draw);
                }
            }

            m_renderer->render();

            m_telemetry.end();
        }
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

    void Application::initWindow(const WindowCreateInfo& inCreateInfo)
    {
        if (hasWindow())
        {
            return;
        }

        m_window = std::make_unique<Window>();
        m_window->init(inCreateInfo);
        m_window->watchSize([&](const Vec<2, int>& inSize) {
            if (hasView())
            {
                m_view->setSize(inSize.x, inSize.y);
            }
        });
    }

    void Application::initRenderer(WindowRenderer inBackend)
    {
        if (hasRenderer())
        {
            return;
        }

        m_renderer = std::make_unique<Renderer::Instance>();
        m_renderer->init(getWindow());
        m_renderer->setBackend(inBackend);
    }
}