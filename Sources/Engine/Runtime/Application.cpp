#include "Chicane/Runtime/Application.hpp"

#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Application
    {
        // Runtime
        static Telemetry g_telemetry = {};

        static Controller*          g_controller           = nullptr;
        static ControllerObservable g_controllerObservable = {};

        static Scene*          g_scene           = nullptr;
        static SceneObservable g_sceneObservable = {};

        // Grid
        static Grid::View*    g_view           = nullptr;
        static ViewObservable g_viewObservable = {};

        // Window
        static WindowCreateInfo        g_windowInfo = {};
        static std::unique_ptr<Window> g_window     = nullptr;

        // Renderer
        static std::unique_ptr<Renderer> g_renderer = nullptr;

        void initWindow()
        {
            if (!hasWindow())
            {
                g_window = std::make_unique<Window>();
                g_window->watchSize([&](const Vec<2, int>& inSize) {
                    if (!g_view)
                    {
                        return;
                    }

                    g_view->setSize(inSize.x, inSize.y);
                });
            }
            else
            {
                g_window->destroy();
            }

            g_window->init(g_windowInfo);
        }

        void initRenderer()
        {
            if (!hasRenderer())
            {
                g_renderer.release();
            }

            switch (g_windowInfo.renderer)
            {
            case WindowRenderer::Vulkan:
                g_renderer = std::make_unique<Vulkan::Renderer>();

                break;

            default:
                g_renderer = std::make_unique<Renderer>();

                break;
            }

            g_renderer->init(g_window.get());
        }

        void initAssets(const String& inPath)
        {
            for (const FileSystem::Item item : FileSystem::ls(inPath.toStandard()))
            {
                if (item.type != FileSystem::ItemType::File)
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

        void render()
        {
            if (hasScene())
            {
                g_scene->tick(g_telemetry.delta);
            }

            if (hasView())
            {
                g_view->tick(g_telemetry.delta);
            }

            if (hasRenderer())
            {
                g_renderer->render();
            }
        }

        void run(const ApplicationCreateInfo& inCreateInfo)
        {
            g_windowInfo = inCreateInfo.window;

            initWindow();
            initRenderer();
            initAssets(".");

            if (inCreateInfo.onSetup)
            {
                inCreateInfo.onSetup();
            }

            while (g_window->run())
            {
                g_telemetry.start();
                render();
                g_telemetry.end();
            }
        }

        const Telemetry& getTelemetry()
        {
            return g_telemetry;
        }

        bool hasController()
        {
            return g_controller != nullptr;
        }

        Controller* getController()
        {
            return g_controller;
        }

        void setController(Controller* inController)
        {
            if (inController == g_controller)
            {
                return;
            }

            g_controller = inController;

            g_controllerObservable.next(g_controller);
        }

        ControllerSubscription watchController(
            ControllerSubscription::NextCallback     inNext,
            ControllerSubscription::ErrorCallback    inError,
            ControllerSubscription::CompleteCallback inComplete
        )
        {
            return g_controllerObservable.subscribe(inNext, inError, inComplete).next(g_controller);
        }

        bool hasScene()
        {
            return g_scene != nullptr;
        }

        Scene* getScene()
        {
            return g_scene;
        }

        void setScene(Scene* inScene)
        {
            if (inScene == g_scene)
            {
                return;
            }

            if (hasScene())
            {
                g_scene->deactivate();
            }

            g_scene = inScene;
            g_scene->activate();

            g_sceneObservable.next(inScene);
        }

        SceneSubscription watchScene(
            SceneSubscription::NextCallback     inNext,
            SceneSubscription::ErrorCallback    inError,
            SceneSubscription::CompleteCallback inComplete
        )
        {
            return g_sceneObservable.subscribe(inNext, inError, inComplete).next(g_scene);
        }

        bool hasView()
        {
            return g_view != nullptr;
        }

        Grid::View* getView()
        {
            return g_view;
        }

        void setView(Grid::View* inView)
        {
            if (inView == g_view)
            {
                return;
            }

            if (hasView())
            {
                g_view->deactivate();
            }

            g_view = inView;
            g_view->setSize(g_window->getSize());
            g_view->activate();

            g_viewObservable.next(g_view);
        }

        ViewSubscription watchView(
            ViewSubscription::NextCallback     inNext,
            ViewSubscription::ErrorCallback    inError,
            ViewSubscription::CompleteCallback inComplete
        )
        {
            return g_viewObservable.subscribe(inNext, inError, inComplete).next(g_view);
        }

        bool hasWindow()
        {
            return g_window && g_window.get() != nullptr;
        }

        Window* getWindow()
        {
            if (!hasWindow())
            {
                return nullptr;
            }

            return g_window.get();
        }

        bool hasRenderer()
        {
            return g_renderer && g_renderer.get() != nullptr;
        }

        Renderer* getRenderer()
        {
            if (!hasRenderer())
            {
                return nullptr;
            }

            return g_renderer.get();
        }

        void setRenderer(WindowRenderer inRenderer)
        {
            g_windowInfo.renderer = inRenderer;

            initWindow();
            initRenderer();
        }
    }
}