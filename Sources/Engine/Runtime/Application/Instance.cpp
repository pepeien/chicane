#include "Chicane/Runtime/Application/Instance.hpp"

#include "Chicane/Runtime/Vulkan/Renderer.hpp"

namespace Chicane
{
    namespace Application
    {
        Instance::Instance()
            : m_telemetry({}),
            m_controller(nullptr),
            m_controllerObservable({}),
            m_scene(nullptr),
            m_sceneObservable({}),
            m_view(nullptr),
            m_viewObservable({})
        {}

        void Instance::init(const CreateInfo& inCreateInfo)
        {
            m_windowInfo = inCreateInfo.window;
            initWindow();

            m_rendererInfo = inCreateInfo.renderer;
            initRenderer();

            initAssets(".");

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

            m_controllerObservable.next(m_controller);
        }

        ControllerSubscription Instance::watchController(
            ControllerSubscription::NextCallback inNext,
            ControllerSubscription::ErrorCallback inError,
            ControllerSubscription::CompleteCallback inComplete
        )
        {
            return m_controllerObservable
                .subscribe(inNext, inError, inComplete)
                .next(m_controller);
        }

        bool Instance::hasScene() const
        {
            return m_scene != nullptr;
        }

        Scene* Instance::getScene() const
        {
            return m_scene;
        }

        void Instance::setScene(Scene* inScene)
        {
            if (inScene == m_scene)
            {
                return;
            }

            if (hasScene())
            {
                m_scene->deactivate();
            }

            m_scene = inScene;
            m_scene->activate();

            m_sceneObservable.next(inScene);
        }

        SceneSubscription Instance::watchScene(
            SceneSubscription::NextCallback inNext,
            SceneSubscription::ErrorCallback inError,
            SceneSubscription::CompleteCallback inComplete
        )
        {
            return m_sceneObservable
                .subscribe(inNext, inError, inComplete)
                .next(m_scene);
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

            m_viewObservable.next(m_view);
        }

        ViewSubscription Instance::watchView(
            ViewSubscription::NextCallback inNext,
            ViewSubscription::ErrorCallback inError,
            ViewSubscription::CompleteCallback inComplete
        )
        {
            return m_viewObservable
                .subscribe(inNext, inError, inComplete)
                .next(m_view);
        }

        bool Instance::hasWindow() const
        {
            return m_window.get() != nullptr;
        }

        bool Instance::hasRenderer() const
        {
            return m_renderer.get() != nullptr;
        }

        void Instance::setRenderer(WindowRenderer inRenderer)
        {
            m_windowInfo.renderer = inRenderer;

            initWindow();
            initRenderer();
        }

        void Instance::initWindow()
        {
            if (!hasWindow())
            {
                m_window = std::make_unique<Window>();
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
            else
            {
                m_window->destroy();
            }

            m_window->init(m_windowInfo);
        }

        void Instance::initRenderer()
        {
            if (!hasRenderer())
            {
                m_renderer.release();
            }

            switch (m_windowInfo.renderer)
            {
            case WindowRenderer::Vulkan:
                m_renderer = std::make_unique<Vulkan::Renderer>();

                break;

            default:
                m_renderer = std::make_unique<Renderer::Instance>();

                break;
            }

            m_renderer->init(m_rendererInfo);
        }

        void Instance::initAssets(const String& inPath)
        {
            for (const FileSystem::Item item : FileSystem::ls(inPath.toStandard()))
            {
                if (item.type != FileSystem::Item::Type::File)
                {
                    initAssets(item.path);

                    continue;
                }

                if (!Box::AssetHeader::isFileAsset(item.path.toStandard()))
                {
                    continue;
                }

                Box::load(item.path.toStandard());
            }
        }

        void Instance::render()
        {
            if (hasRenderer())
            {
                m_renderer->render();
            }

            if (hasView())
            {
                m_view->tick(m_telemetry.delta);
            }

            if (hasScene())
            {
                m_scene->tick(m_telemetry.delta);
            }
        }
    }
}