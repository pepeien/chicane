#include "Chicane/Runtime/Application/Instance.hpp"

#include "Chicane/Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Application
    {
        Instance::Instance()
            : m_telemetry({}),
            m_controller(nullptr),
            m_controllerObservable(std::make_unique<Observable<Controller::Instance*>>()),
            m_level(nullptr),
            m_levelObservable(std::make_unique<Observable<Level*>>())
        {}

        void Instance::setup(const CreateInfo& inCreateInfo)
        {
            initWindow(inCreateInfo.window, inCreateInfo.renderer.type);
            initRenderer(inCreateInfo.renderer);
            initAssets(".");

            Grid::watchActiveView(
                [this](Grid::View* inView)
                {
                    if (!inView)
                    {
                        return;
                    }

                    inView->setWindow(m_window.get());
                }
            );

            if (inCreateInfo.onSetup)
            {
                inCreateInfo.onSetup();
            }
        }

        void Instance::run()
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

        const Telemetry& Instance::getTelemetry()
        {
            return m_telemetry;
        }

        bool Instance::hasController()
        {
            return  m_controller != nullptr;
        }

        Controller::Instance* Instance::getController()
        {
            return  m_controller;
        }

        void Instance::setController(Controller::Instance* inController)
        {
            if (inController == m_controller)
            {
                return;
            }

            m_controller = inController;

            m_controllerObservable->next(m_controller);
        }

        Subscription<Controller::Instance*>* Instance::watchController(
            std::function<void (Controller::Instance*)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        )
        {
            Subscription<Controller::Instance*>* subscription = m_controllerObservable->subscribe(
                inNext,
                inError,
                inComplete
            );
            subscription->next(m_controller);

            return subscription;
        }

        bool Instance::hasLevel()
        {
            return m_level != nullptr;
        }

        Level* Instance::getLevel()
        {
            return m_level;
        }

        void Instance::setLevel(Level* inLevel)
        {
            if (inLevel == m_level)
            {
                return;
            }

            m_level = inLevel;

            m_levelObservable->next(inLevel);
        }

        Subscription<Level*>* Instance::watchLevel(
            std::function<void (Level*)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete 
        )
        {
            Subscription<Level*>* subscription = m_levelObservable->subscribe(
                inNext,
                inError,
                inComplete
            );
            subscription->next(m_level);

            return subscription;
        }

        bool Instance::hasWindow()
        {
            return m_window.get() != nullptr;
        }

        bool Instance::hasRenderer()
        {
            return m_renderer.get() != nullptr;
        }

        void Instance::initWindow(const Window::CreateInfo& inCreateInfo, Renderer::Type inRendererType)
        {
            if (hasWindow())
            {
                return;
            }

            m_window = std::make_unique<Window::Instance>(inCreateInfo, inRendererType);
        }

        void Instance::initRenderer(const Renderer::CreateInfo& inCreateInfo)
        {
            if (!hasWindow() || hasRenderer())
            {
                return;
            }

            switch (inCreateInfo.type)
            {
            case Renderer::Type::Vulkan:
                m_renderer = std::make_unique<Vulkan::Renderer>(inCreateInfo, m_window.get());

                break;

            default:
                m_renderer = std::make_unique<Renderer::Instance>(inCreateInfo, m_window.get());

                break;
            }

            m_renderer->initLayers();
        }

        void Instance::initAssets(const std::string& inPath)
        {
            for (const FileSystem::Item item : FileSystem::ls(inPath))
            {
                if (item.type != FileSystem::Item::Type::File)
                {
                    initAssets(item.path);

                    continue;
                }

                if (!Box::Asset::isFileAsset(item.path))
                {
                    continue;
                }

                Box::load(item.path);
            }
        }

        void Instance::onEvent(const SDL_Event& inEvent) {
            if (m_window)
            {
                m_window->onEvent(inEvent);
            }

            if (m_renderer)
            {
                m_renderer->onEvent(inEvent);
            }

            if (m_controller)
            {
                m_controller->onEvent(inEvent);
            }

            if (Grid::hasActiveView())
            {
                Grid::getActiveView()->onEvent(inEvent);
            }
        }

        void Instance::onRender()
        {
            m_telemetry.start();
                if (hasRenderer())
                {
                    m_renderer->render();
                }

                if (hasLevel())
                {
                    m_level->tick(m_telemetry.delta);
                }

                if (Grid::hasActiveView())
                {
                    Grid::getActiveView()->tick(m_telemetry.delta);
                }
            m_telemetry.end();
        }
    }
}