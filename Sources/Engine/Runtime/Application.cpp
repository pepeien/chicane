#include "Chicane/Runtime/Application.hpp"

#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Model/Manager.hpp"

#include "Chicane/Kerb.hpp"

#include "Chicane/Renderer/Instance.hpp"

#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

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
        static std::unique_ptr<Window> g_window = nullptr;

        // Renderer
        static std::unique_ptr<Renderer::Instance> g_renderer = nullptr;

        void initWindow(const WindowCreateInfo& inCreateInfo)
        {
            if (hasWindow())
            {
                return;
            }

            g_window = std::make_unique<Window>();
            g_window->init(inCreateInfo);
            g_window->watchSize([&](const Vec<2, int>& inSize) {
                if (hasView())
                {
                    g_view->setSize(inSize.x, inSize.y);
                }

                if (hasRenderer())
                {
                    g_renderer->setSize(inSize.x, inSize.y);
                }
            });
            g_window->watchEvent([&](WindowEvent inEvent) {
                if (hasRenderer())
                {
                    g_renderer->handle(inEvent);
                }
            });
        }

        void initRenderer()
        {
            if (hasRenderer())
            {
                return;
            }

            g_renderer = std::make_unique<Renderer::Instance>();
            g_renderer->init(getWindow());
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
                g_renderer->useCamera(g_scene->getActiveComponents<CCamera>().at(0)->getData());
                g_renderer->addLight(g_scene->getActiveComponents<CLight>().at(0)->getData());

                CMesh*              mesh  = g_scene->getActiveComponents<CMesh>().at(0);
                Box::ModelExtracted model = Box::getModelManager()->getInstance(
                    mesh->getMesh()->getGroups().at(0).getModel()
                );
                Renderer::DrawData3D draw = {};
                draw.indices              = model.indices;
                draw.vertices             = model.vertices;
                draw.model                = mesh->getTransform().getMatrix();

                g_renderer->draw(draw);

                g_renderer->render();
            }
        }

        void run(const ApplicationCreateInfo& inCreateInfo)
        {
            Box::init();
            Kerb::init();

            initWindow(inCreateInfo.window);
            initRenderer();

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

        Renderer::Instance* getRenderer()
        {
            if (!hasRenderer())
            {
                return nullptr;
            }

            return g_renderer.get();
        }
    }
}