#include "Chicane/Application.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

namespace Chicane
{
    static const Telemetry EMPTY_TRANSFORM = {};

    std::unique_ptr<Application> m_application = nullptr;

    Application::Application()
        : telemetry({}),
        controller(nullptr),
        controllerObservable(std::make_unique<Observable<Controller*>>()),
        level(nullptr),
        levelObservable(std::make_unique<Observable<Level*>>()),
        defaultCamera(nullptr),
        camera(nullptr),
        cameraObservable(std::make_unique<Observable<CameraComponent*>>())
    {}

    void Application::start(const CreateInfo& inCreateInfo)
    {
        if (m_application)
        {
            return;
        }

        m_application = std::make_unique<Application>();
        m_application->init(inCreateInfo);
        m_application->run();
    }

    const Telemetry& Application::getTelemetry()
    {
        if (!m_application)
        {
            return EMPTY_TRANSFORM;
        }

        return m_application->telemetry;
    }

    bool Application::hasController()
    {
        if (!m_application)
        {
            return false;
        }

        return  m_application->controller != nullptr;
    }

    Controller* Application::getController()
    {
        if (!m_application)
        {
            return nullptr;
        }

        return  m_application->controller;
    }

    void Application::setController(Controller* inController)
    {
        if (!m_application || inController == m_application->controller)
        {
            return;
        }

        m_application->controller = inController;

        m_application->controllerObservable->next(m_application->controller);
    }

    void Application::watchController(
        std::function<void (Controller*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        if (!m_application)
        {
            return;
        }

        m_application->controllerObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        )->next(m_application->controller);
    }

    bool Application::hasLevel()
    {
        if (!m_application)
        {
            return false;
        }

        return m_application->level != nullptr;
    }

    Level* Application::getLevel()
    {
        if (!m_application)
        {
            return nullptr;
        }

        return m_application->level;
    }

    void Application::setLevel(Level* inLevel)
    {
        if (!m_application || inLevel == m_application->level)
        {
            return;
        }

        m_application->level = inLevel;

        m_application->levelObservable->next(m_application->level);

        if (!m_application->hasLevel())
        {
            return;
        }

        m_application->level->onActivation();
    }

    void Application::watchLevel(
        std::function<void (Level*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback 
    )
    {
        if (!m_application)
        {
            return;
        }

        m_application->levelObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        )->next(m_application->level);
    }

    bool Application::hasCamera()
    {
        if (!m_application)
        {
            return false;
        }

        return m_application->camera != nullptr;
    }

    CameraComponent* Application::getCamera()
    {
        if (!m_application)
        {
            return nullptr;
        }

        return m_application->camera;
    }

    void Application::setCamera(CameraComponent* inCamera)
    {
        if (!m_application || inCamera == m_application->camera)
        {
            return;
        }

        m_application->camera = inCamera;

        m_application->cameraObservable->next(inCamera);
    }

    void Application::watchCamera(
        std::function<void (CameraComponent*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        if (!m_application)
        {
            return;
        }

        m_application->cameraObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        )->next(m_application->camera);
    }

    bool Application::hasWindow()
    {
        if (!m_application)
        {
            return false;
        }

        return m_application->window != nullptr;
    }

    Window* Application::getWindow()
    {
        if (!m_application)
        {
            return nullptr;
        }

        return m_application->window.get();
    }

    bool Application::hasRenderer()
    {
        if (!m_application)
        {
            return false;
        }

        return m_application->renderer != nullptr;
    }

    Renderer* Application::getRenderer()
    {
        if (!m_application)
        {
            return nullptr;
        }

        return m_application->renderer.get();
    }

    void Application::init(const CreateInfo& inCreateInfo)
    {
        initWindow(inCreateInfo.windowCreateInfo);
        initCamera();
        initRenderer(inCreateInfo.renderer);
        initLayers();
    }

    void Application::run()
    {
        SDL_Event event;

        while (true)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                {
                    return;
                }

                onEvent(event);
            }

            onRender();
        }
    }

    void Application::initWindow(const Window::CreateInfo& inWindowCreateInfo)
    {
        if (Application::hasWindow())
        {
            return;
        }

        window = std::make_unique<Window>(inWindowCreateInfo);
    }

    void Application::initCamera()
    {
        if (!Application::hasWindow())
        {
            return;
        }

        defaultCamera = new CameraComponent();

        cameraObservable->subscribe(
            [this](CameraComponent* inCamera) {
                if (!inCamera)
                { 
                    defaultCamera->setAbsoluteTranslation(Vec<3, float>(0.0f, 0.0f, 100.0f));
                    defaultCamera->activate();

                    return;
                }
            }
        );
    }

    void Application::initRenderer(Renderer::Type inRendererType)
    {
        if (!Application::hasWindow() || Application::hasRenderer())
        {
            return;
        }

        switch (inRendererType)
        {
        case Renderer::Type::Vulkan :
            renderer = std::make_unique<Vulkan::Renderer>(window.get());

            break;

        default:
            renderer = std::make_unique<Renderer>(window.get());

            break;
        }
    }

    void Application::initLayers()
    {
        if (!Application::hasWindow() || !Application::hasRenderer())
        {
            return;
        }

        renderer->pushLayer(new SkyboxLayer());
        renderer->pushLayer(new LevelLayer());
        renderer->pushLayer(new UILayer());
    }

    void Application::onEvent(const SDL_Event& inEvent) {
        window->onEvent(inEvent);
        renderer->onEvent(inEvent);

        if (Application::hasController())
        {
            getController()->onEvent(inEvent);
        }
    }

    void Application::onRender()
    {
        telemetry.startCapture();
            renderer->render();
        telemetry.endCapture();

        float delta = telemetry.frame.deltaToTick();

        if (Application::hasLevel())
        {
            getLevel()->onTick(delta);
        }
    }
}