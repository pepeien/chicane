#pragma once

#include <atomic>
#include <mutex>
#include <thread>

#include "Chicane/Core/Telemetry.hpp"
#include "Chicane/Core/Window.hpp"

#include "Chicane/Grid/Component/View.hpp"

#include "Chicane/Renderer/Draw/Poly/2D/Command.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Command.hpp"
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

        using SceneObservable   = EventObservable<std::shared_ptr<Scene>>;
        using SceneSubscription = EventSubscription<std::shared_ptr<Scene>>;

        using ViewObservable   = EventObservable<std::shared_ptr<Grid::View>>;
        using ViewSubscription = EventSubscription<std::shared_ptr<Grid::View>>;

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

        template <class T = Scene>
        std::shared_ptr<T> getScene()
        {
            std::shared_ptr<Scene> scene = std::atomic_load_explicit(&m_scene, std::memory_order_acquire);

            if (!scene)
            {
                return nullptr;
            }

            return std::static_pointer_cast<T>(scene);
        }
        template <class T, typename... Params>
        void setScene(Params... inParams)
        {
            std::shared_ptr<Scene> scene = std::make_shared<T>(std::forward<Params>(inParams)...);
            scene->activate();

            std::atomic_store_explicit(&m_scene, scene, std::memory_order_release);

            m_sceneObservable.next(getScene());
        }
        SceneSubscription watchScene(
            SceneSubscription::NextCallback     inNext,
            SceneSubscription::ErrorCallback    inError    = nullptr,
            SceneSubscription::CompleteCallback inComplete = nullptr
        );

        // UI
        template <class T = Grid::View>
        std::shared_ptr<T> getView()
        {
            std::shared_ptr<Grid::View> view = std::atomic_load_explicit(&m_view, std::memory_order_acquire);

            if (!view)
            {
                return nullptr;
            }

            return std::static_pointer_cast<T>(view);
        }
        template <class T, typename... Params>
        void setView(Params... inParams)
        {
            std::shared_ptr<Grid::View> view = std::make_shared<T>(std::forward<Params>(inParams)...);

            std::atomic_store_explicit(&m_view, view, std::memory_order_release);

            m_viewObservable.next(getView());
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
        // Renderer
        void initRenderer(const Renderer::Settings& inSettings);
        void shutdownRenderer();

        // Window
        void initWindow(const WindowSettings& inSettings);

        // Box
        void initBox();

        // Kerb
        void initKerb();

        // Lifecycle
        void render();

        // Scene
        void initScene();
        void shutdownScene();
        void tickScene();
        void buildSceneCommands(std::shared_ptr<Scene> inScene);
        void renderScene();

        // Grid
        void initView();
        void shutdownView();
        void tickView();
        void buildViewCommands(std::shared_ptr<Grid::View> inView);
        void renderView();

    private:
        // Status
        Telemetry                                      m_telemetry;
        std::atomic<bool>                              m_bIsRunning;

        // Scene
        Controller*                                    m_controller;
        ControllerObservable                           m_controllerObservable;

        std::shared_ptr<Scene>                         m_scene;
        std::thread                                    m_sceneThread;
        std::vector<Renderer::DrawPoly3DCommand::List> m_sceneCommandBuffers;
        std::atomic<uint32_t>                          m_sceneWriteIndex;
        std::atomic<uint32_t>                          m_sceneReadIndex;
        SceneObservable                                m_sceneObservable;

        // Grid
        std::shared_ptr<Grid::View>                    m_view;
        std::thread                                    m_viewThread;
        std::vector<Renderer::DrawPoly2DCommand::List> m_viewCommandBuffers;
        std::atomic<uint32_t>                          m_viewWriteIndex;
        std::atomic<uint32_t>                          m_viewReadIndex;
        ViewObservable                                 m_viewObservable;

        // Window
        std::unique_ptr<Window>                        m_window;

        // Renderer
        std::unique_ptr<Renderer::Instance>            m_renderer;
    };
}