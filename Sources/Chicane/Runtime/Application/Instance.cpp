#include "Chicane/Runtime/Application/Instance.hpp"

#include "Chicane/Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Application
    {
        Instance::Instance()
            : m_telemetry({}),
            m_controller(nullptr),
            m_controllerObservable(std::make_unique<ControllerObservable>()),
            m_level(nullptr),
            m_levelObservable(std::make_unique<LevelObservable>()),
            m_view(nullptr),
            m_viewObservable(std::make_unique<ViewObservable>())
        {}

        void Instance::setup(const CreateInfo& inCreateInfo)
        {
            m_windowInfo = inCreateInfo.window;
            if (!hasWindow())
            {
                initWindow();
                m_window->init(m_windowInfo, inCreateInfo.renderer.type);
            }

            m_rendererInfo = inCreateInfo.renderer;
            if (!hasRenderer())
            {
                initRenderer();
                m_renderer->init(m_rendererInfo);
            }

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

        ControllerSubscription* Instance::watchController(
            ControllerSubscription::NextCallback inNext,
            ControllerSubscription::ErrorCallback inError,
            ControllerSubscription::CompleteCallback inComplete
        ) const
        {
            return m_controllerObservable
                ->subscribe(inNext, inError, inComplete)
                ->next(m_controller);
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

        LevelSubscription* Instance::watchLevel(
            LevelSubscription::NextCallback inNext,
            LevelSubscription::ErrorCallback inError,
            LevelSubscription::CompleteCallback inComplete
        ) const
        {
            return m_levelObservable
                ->subscribe(inNext, inError, inComplete)
                ->next(m_level);
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
            m_view->setSize(m_window->getSize());
            m_view->activate();

            m_viewObservable->next(m_view);
        }

        ViewSubscription* Instance::watchView(
            ViewSubscription::NextCallback inNext,
            ViewSubscription::ErrorCallback inError,
            ViewSubscription::CompleteCallback inComplete
        ) const
        {
            return m_viewObservable
                ->subscribe(inNext, inError, inComplete)
                ->next(m_view);
        }

        bool Instance::hasWindow() const
        {
            return m_window.get() != nullptr;
        }

        bool Instance::hasRenderer() const
        {
            return m_renderer.get() != nullptr;
        }

        void Instance::setRenderer(Renderer::Type inType)
        {
            if (!hasWindow())
            {
                return;
            }

            if (hasRenderer())
            {
                m_renderer.reset();
            }

            m_rendererInfo.type = inType;

            m_window->destroy();
            m_window->init(m_windowInfo, m_rendererInfo.type);

            initRenderer();
        }

        void Instance::initWindow()
        {
            if (hasWindow())
            {
                return;
            }

            m_window = std::make_unique<Window::Instance>();

            m_window->watchSize(
                [this](const Vec<2, int>& inSize)
                {
                    if (!m_view)
                    {
                        return;
                    }

                    m_view->setSize(inSize.x, inSize.y);
                }
            );
        }

        void Instance::initRenderer()
        {
            if (hasRenderer())
            {
                return;
            }

            switch (m_rendererInfo.type)
            {
            case Renderer::Type::Vulkan:
                m_renderer = std::make_unique<Vulkan::Renderer>();

                break;

            default:
                m_renderer = std::make_unique<Renderer::Instance>();

                break;
            }
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
            if (hasWindow())
            {
                m_window->handle(inEvent);
            }

            if (hasRenderer())
            {
                m_renderer->handle(inEvent);
            }

            if (hasView())
            {
                m_view->handle(inEvent);
            }

            if (hasController())
            {
                m_controller->handle(inEvent);
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