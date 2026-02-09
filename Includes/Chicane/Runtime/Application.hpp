#pragma once

#include <atomic>
#include <mutex>
#include <thread>

#include "Chicane/Core/Telemetry.hpp"
#include "Chicane/Core/Window.hpp"

#include "Chicane/Grid/Component/View.hpp"

#include "Chicane/Renderer/Instance.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Application/CreateInfo.hpp"
#include "Chicane/Runtime/Controller.hpp"
#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME Application
    {
    public:
        using ControllerObservable   = EventObservable<Controller*>;
        using ControllerSubscription = EventSubscription<Controller*>;

        using SceneObservable   = EventObservable<Scene*>;
        using SceneSubscription = EventSubscription<Scene*>;

        using ViewObservable   = EventObservable<Grid::View*>;
        using ViewSubscription = EventSubscription<Grid::View*>;

    public:
        static inline Application& getInstance()
        {
            static Application instance;

            return instance;
        }

    public:
        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

        Application(Application&&)            = delete;
        Application& operator=(Application&&) = delete;

    private:
        Application();
        ~Application();

    public:
        // Lifecycle
        void run(const ApplicationCreateInfo& inCreateInfo);

        // Telemetry
        const Telemetry& getTelemetry() const;

        // Game
        bool hasController();
        Controller* getController();
        void setController(Controller* inController);
        ControllerSubscription watchController(
            ControllerSubscription::NextCallback     inNext,
            ControllerSubscription::ErrorCallback    inError    = nullptr,
            ControllerSubscription::CompleteCallback inComplete = nullptr
        );

        bool hasScene();
        template <class T = Scene, typename... Params>
        void setScene(Params... inParams)
        {
            if (hasScene())
            {
                m_scene->deactivate();
                m_scene.reset();
            }

            m_scene = std::make_unique<T>(inParams...);
            m_scene->activate();

            m_sceneObservable.next(m_scene.get());
        }
        template <class T = Scene>
        T* getScene()
        {
            if (!hasScene())
            {
                return nullptr;
            }

            return static_cast<T*>(m_scene.get());
        }
        SceneSubscription watchScene(
            SceneSubscription::NextCallback     inNext,
            SceneSubscription::ErrorCallback    inError    = nullptr,
            SceneSubscription::CompleteCallback inComplete = nullptr
        );

        // UI
        bool hasView();
        template <class T = Scene>
        T* getView()
        {
            if (!hasScene())
            {
                return nullptr;
            }

            return static_cast<T*>(m_scene.get());
        }
        template <class T, typename... Params>
        void setView(Params... inParams)
        {
            if (hasView())
            {
                m_view->deactivate();
                m_view.reset();
            }

            m_view = std::make_unique<T>(inParams...);
            m_view->activate();

            if (hasRenderer())
            {
                m_view->setSize(m_renderer->getResolution());
            }

            m_viewObservable.next(m_view.get());
        }
        ViewSubscription watchView(
            ViewSubscription::NextCallback     inNext,
            ViewSubscription::ErrorCallback    inError    = nullptr,
            ViewSubscription::CompleteCallback inComplete = nullptr
        );

        // Window
        bool hasWindow();
        Window* getWindow();

        // Renderer
        bool hasRenderer();
        Renderer::Instance* getRenderer();
        void setRenderer(WindowBackend inBackend);

    private:
        // Initialization
        void initRenderer(const Renderer::Settings& inSettings);
        void initWindow(const WindowSettings& inSettings);
        void initBox();
        void initKerb();

        // Lifecycle
        void render();

        void initSceneThread();
        void shutdownSceneThread();
        void tickScene();
        void renderScene();

        void initViewThread();
        void shutdownViewThread();
        void tickView();
        void renderView();

    private:
        // Status
        Telemetry                           m_telemetry;

        // Scene
        Controller*                         m_controller;
        ControllerObservable                m_controllerObservable;

        std::unique_ptr<Scene>              m_scene;
        std::atomic<bool>                   m_bIsSceneRunning;
        std::mutex                          m_sceneMutex;
        std::thread                         m_sceneThread;
        Timer                               m_sceneTimer;
        SceneObservable                     m_sceneObservable;

        // Grid
        std::unique_ptr<Grid::View>         m_view;
        std::atomic<bool>                   m_bIsViewRunning;
        std::mutex                          m_viewMutex;
        std::thread                         m_viewThread;
        Timer                               m_viewTimer;
        ViewObservable                      m_viewObservable;

        // Window
        std::unique_ptr<Window>             m_window;

        // Renderer
        std::unique_ptr<Renderer::Instance> m_renderer;
    };
}