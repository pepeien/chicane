#include "Chicane/Application.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Renderer/Vulkan.hpp"

namespace Chicane
{
    static const Telemetry EMPTY_TRANSFORM = {};

    std::unique_ptr<Application> m_application = nullptr;

    void Application::run(const CreateInfo& inCreateInfo)
    {
        if (m_application)
        {
            return;
        }

        m_application = std::make_unique<Application>();
        m_application->setup(inCreateInfo);
        m_application->run();
    }

    void Application::stop()
    {
        if (!m_application)
        {
            return;
        }

        m_application.reset();
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
        std::function<void (Controller*)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        if (!m_application)
        {
            return;
        }

        m_application->controllerObservable->subscribe(
            inNext,
            inError,
            inComplete
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

        m_application->levelObservable->next(inLevel);
    }

    void Application::watchLevel(
        std::function<void (Level*)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete 
    )
    {
        if (!m_application)
        {
            return;
        }

        m_application->levelObservable->subscribe(
            inNext,
            inError,
            inComplete
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
        std::function<void (CameraComponent*)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        if (!m_application)
        {
            return;
        }

        m_application->cameraObservable->subscribe(
            inNext,
            inError,
            inComplete
        )->next(m_application->camera);
    }

    bool Application::hasView()
    {
        if (!m_application)
        {
            return false;
        }

        return m_application->view != nullptr;
    }

    Grid::View* Application::getView(const std::string& inId)
    {
        if (!m_application)
        {
            return nullptr;
        }

        if (inId.empty())
        {
            return m_application->view;
        }

        if (m_application->views.find(inId) == m_application->views.end())
        {
            return nullptr;
        }

        return m_application->views.at(inId);
    }

    void Application::addView(Grid::View* inView)
    {
        if (!inView)
        {
            return;
        }

        if (!m_application || m_application->views.find(inView->getId()) != m_application->views.end())
        {
            return;
        }

        m_application->views.insert(
            std::make_pair(
                inView->getId(),
                inView
            )
        );
    }

    void Application::setView(const std::string& inId)
    {
        if (!m_application || inId.empty())
        {
            return;
        }

        Grid::View* view = Application::getView(inId);

        if (m_application->view == view)
        {
            return;
        }

        m_application->view = view;

        m_application->viewObservable->next(view);
    }

    void Application::watchView(
        std::function<void (Grid::View*)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        if (!m_application)
        {
            return;
        }

        m_application->viewObservable->subscribe(
            inNext,
            inError,
            inComplete
        )->next(m_application->view);
    }

    bool Application::hasWindow()
    {
        if (!m_application)
        {
            return false;
        }

        return m_application->window.get() != nullptr;
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

        return m_application->renderer.get() != nullptr;
    }

    Renderer* Application::getRenderer()
    {
        if (!m_application)
        {
            return nullptr;
        }

        return m_application->renderer.get();
    }

    Application::Application()
        : telemetry({}),
        controller(nullptr),
        controllerObservable(std::make_unique<Observable<Controller*>>()),
        level(nullptr),
        levelObservable(std::make_unique<Observable<Level*>>()),
        defaultCamera(nullptr),
        camera(nullptr),
        cameraObservable(std::make_unique<Observable<CameraComponent*>>()),
        views({}),
        view(nullptr),
        viewObservable(std::make_unique<Observable<Grid::View*>>())
    {}

    void Application::setup(const CreateInfo& inCreateInfo)
    {
        initWindow(inCreateInfo);
        initRenderer(inCreateInfo.renderer);
        initDefaultController();
        initDefaultLevel();
        initDefaultCamera();
        initDefaultLayers();
        initEvents();

        if (inCreateInfo.onSetup)
        {
            inCreateInfo.onSetup();
        }
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

    void Application::initRenderer(RendererType inRendererType)
    {
        if (!Application::hasWindow() || Application::hasRenderer())
        {
            return;
        }

        switch (inRendererType)
        {
        case RendererType::Vulkan:
            renderer = std::make_unique<Vulkan::Renderer>(window.get());

            break;

        default:
            renderer = std::make_unique<Renderer>(window.get());

            break;
        }
    }

    void Application::initDefaultController()
    {
        if (!Application::hasWindow())
        {
            return;
        }

        defaultController = std::make_unique<Controller>();
    }

    void Application::initDefaultLevel()
    {
        if (!Application::hasWindow())
        {
            return;
        }

        defaultLevel = std::make_unique<Level>();
    }

    void Application::initDefaultCamera()
    {
        if (!Application::hasWindow())
        {
            return;
        }

        defaultCamera = new CameraComponent();
    }

    void Application::initDefaultLayers()
    {
        if (!Application::hasWindow() || !Application::hasRenderer())
        {
            return;
        }

        renderer->initLayers();
    }

    void Application::initEvents()
    {
        Application::watchController(
            [this](Controller* inController) {
                if (!inController)
                {
                    defaultController->activate();

                    return;
                }
            }
        );

        Application::watchLevel(
            [this](Level* inLevel) {
                if (!inLevel)
                {
                    defaultLevel->activate();

                    return;
                }
            }
        );

        Application::watchCamera(
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

    void Application::onEvent(const SDL_Event& inEvent) {
        if (window)
        {
            window->onEvent(inEvent);
        }

        if (renderer)
        {
            renderer->onEvent(inEvent);
        }

        if (controller)
        {
            controller->onEvent(inEvent);
        }
    }

    void Application::onRender()
    {
        telemetry.startCapture();
            if (renderer)
            {
                renderer->render();
            }
        telemetry.endCapture();

        if (level)
        {
            level->onTick(telemetry.frame.deltaToTick());
        }
    }
}