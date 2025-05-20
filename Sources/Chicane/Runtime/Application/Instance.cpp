#include "Chicane/Runtime/Application/Instance.hpp"

#include "Chicane/Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Application
    {
        Instance::Instance()
            : m_telemetry({}),
            m_controller(nullptr),
            m_controllerObservable(std::make_unique<Observable<Controller*>>()),
            m_level(nullptr),
            m_levelObservable(std::make_unique<Observable<Level*>>()),
            m_view(nullptr),
            m_viewObservable(std::make_unique<Observable<Grid::View*>>())
        {}

        void Instance::setup(const CreateInfo& inCreateInfo)
        {
            initWindow(inCreateInfo.window, inCreateInfo.renderer.type);
            initRenderer(inCreateInfo.renderer);
            initAssets(".");

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

        const Telemetry& Instance::getTelemetry() const
        {
            return m_telemetry;
        }

        bool Instance::hasController() const
        {
            return  m_controller != nullptr;
        }

        Controller* Instance::getController() const
        {
            return  m_controller;
        }

        void Instance::setController(Controller* inController)
        {
            if (inController == m_controller)
            {
                return;
            }

            m_controller = inController;

            m_controllerObservable->next(m_controller);
        }

        Subscription<Controller*>* Instance::watchController(
            std::function<void (Controller*)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        ) const
        {
            inNext(m_controller);

            return m_controllerObservable->subscribe(inNext, inError,inComplete);
        }

        bool Instance::hasLevel() const
        {
            return m_level != nullptr;
        }

        Level* Instance::getLevel() const
        {
            return m_level;
        }

        void Instance::setLevel(Level* inLevel)
        {
            if (inLevel == m_level)
            {
                return;
            }

            if (hasLevel())
            {
                m_level->deactivate();
            }

            m_level = inLevel;
            m_level->activate();

            m_levelObservable->next(inLevel);
        }

        Subscription<Level*>* Instance::watchLevel(
            std::function<void (Level*)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete 
        ) const
        {
            inNext(m_level);

            return m_levelObservable->subscribe(inNext, inError,inComplete);
        }

        bool Instance::hasView() const
        {
            return m_view != nullptr;
        }

        Grid::View* Instance::getView() const
        {
            return m_view;
        }

        void Instance::setView(Grid::View* inView)
        {
            if (inView == m_view)
            {
                return;
            }

            if (hasView())
            {
                m_view->deactivate();
            }

            m_view = inView;
            m_view->setWindow(m_window.get());
            m_view->activate();

            m_viewObservable->next(m_view);
        }

        Subscription<Grid::View*>* Instance::watchView(
            std::function<void (Grid::View*)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        ) const
        {
            inNext(m_view);

            return m_viewObservable->subscribe(inNext, inError, inComplete);
        }

        bool Instance::hasWindow() const
        {
            return m_window.get() != nullptr;
        }

        bool Instance::hasRenderer() const
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

            if (hasView())
            {
                m_view->onEvent(inEvent);
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

                if (hasView())
                {
                    m_view->tick(m_telemetry.delta);
                }
            m_telemetry.end();
        }
    }
}