#pragma once

#include <atomic>
#include <thread>
#include <vector>

#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Window.hpp"

#include "Chicane/Grid/Component/View.hpp"

#include "Chicane/Renderer/Draw/Poly/2D/Command.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Command.hpp"
#include "Chicane/Renderer/Instance.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Instance/CreateInfo.hpp"
#include "Chicane/Runtime/Instance/Telemetry.hpp"
#include "Chicane/Runtime/Controller.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Trace/Request.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME Instance
    {
    public:
        using ControllerObservable   = EventObservable<Controller*>;
        using ControllerSubscription = EventSubscription<Controller*>;

        using SceneObservable   = EventObservable<std::shared_ptr<Scene>>;
        using SceneSubscription = EventSubscription<std::shared_ptr<Scene>>;

        using ViewObservable   = EventObservable<std::shared_ptr<Grid::View>>;
        using ViewSubscription = EventSubscription<std::shared_ptr<Grid::View>>;

    public:
        static Instance& sInstance();

    public:
        Instance(const Instance&)            = delete;
        Instance& operator=(const Instance&) = delete;

        Instance(Instance&&)            = delete;
        Instance& operator=(Instance&&) = delete;

    private:
        Instance();

    public:
        // Lifecycle
        void run(const InstanceCreateInfo& inCreateInfo);

        // Telemetry
        const InstanceTelemetry& getTelemetry() const;

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
            if (std::shared_ptr<Scene> currentScene = getScene())
            {
                currentScene->unload();
            }

            std::shared_ptr<Scene> scene = std::make_shared<T>(std::forward<Params>(inParams)...);
            scene->load();

            std::atomic_store_explicit(&m_scene, scene, std::memory_order_release);

            m_sceneObservable.next(getScene());
        }

        void setScene(std::shared_ptr<Scene> inScene)
        {
            if (!inScene || getScene() == inScene)
            {
                return;
            }

            std::atomic_store_explicit(&m_scene, inScene, std::memory_order_release);

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
        bool hasWindow() const;
        Window* getWindow() const;

        // Viewport
        Vec<2, std::uint32_t> getScreenViewport() const;
        Bounds2D getScreenViewportRect() const;

        // Renderer
        bool hasRenderer() const;
        Renderer::Instance* getRenderer() const;
        void setRenderer(WindowBackend inBackend);

        // Debug
        void pushTrace(const SceneTraceRequest& inRequest);
        void pushTrace(const SceneTraceRequest& inRequest, const Vec4& inColor);

    private:
        // Renderer
        void initRenderer(const Renderer::Settings& inSettings);
        void shutdownRenderer();

        // Window
        void initWindow(const WindowSettings& inSettings);

        // Systems
        void initModules(const std::vector<FileSystem::Path>& inModules);
        void shutdownModules();

        void initBox();

        void initKerb();

        void initDrift();
        void shutdownDrift();

        void initScreech();

        void initSmoke();
        void shutdownSmoke();

        // Lifecycle
        void render();
        void uploadPreviewTextures();
        void snapshotRendererState();
        bool hasSceneFeature(Renderer::RendererFeature inFeature) const;
        Vec<2, std::uint32_t> getRendererResolution() const;

        // Scene
        void initScene();
        void shutdownScene();
        void tickScene();

        void buildSceneCommands(std::shared_ptr<Scene> inScene);
        void renderScene();

        // Grid
        void initUI();
        void shutdownUI();
        void tickUI();

        void snapshotScreenViewport(const std::shared_ptr<Grid::View>& inView);

        void buildUICommands(std::shared_ptr<Grid::View> inView);
        void renderUI();

    private:
        // Status
        InstanceTelemetry                        m_telemetry;
        std::atomic<bool>                        m_bIsRunning;

        // Scene
        Controller*                              m_controller;
        ControllerObservable                     m_controllerObservable;

        std::shared_ptr<Scene>                   m_scene;
        std::thread                              m_sceneThread;
        std::vector<Renderer::DrawPoly3DCommand> m_sceneCommandBuffers;
        std::atomic<std::size_t>                 m_sceneWriteIndex;
        std::atomic<std::size_t>                 m_sceneReadIndex;
        std::atomic<std::size_t>                 m_sceneBusyIndex;
        SceneObservable                          m_sceneObservable;

        // Grid
        std::shared_ptr<Grid::View>              m_view;
        std::thread                              m_viewThread;
        std::vector<Renderer::DrawPoly2DCommand> m_viewCommandBuffers;
        std::vector<Grid::Component*>            m_viewDrawables;
        std::atomic<std::size_t>                 m_viewWriteIndex;
        std::atomic<std::size_t>                 m_viewReadIndex;
        std::atomic<std::uint32_t>               m_screenViewportX;
        std::atomic<std::uint32_t>               m_screenViewportY;
        std::atomic<std::uint32_t>               m_screenViewportWidth;
        std::atomic<std::uint32_t>               m_screenViewportHeight;
        ViewObservable                           m_viewObservable;

        // Window
        std::unique_ptr<Window>                  m_window;

        // Renderer
        std::unique_ptr<Renderer::Instance>      m_renderer;
        std::atomic<std::uint16_t>               m_featureFlags;
        std::atomic<std::uint32_t>               m_rendererWidth;
        std::atomic<std::uint32_t>               m_rendererHeight;
    };
}
