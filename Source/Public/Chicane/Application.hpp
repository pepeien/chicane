#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    class Application
    {
    public:
        struct CreateInfo
        {
        public:
            Window::CreateInfo windowCreateInfo = {};
            Renderer::Type     renderer         = Renderer::Type::Undefined;
        };

    public:
        Application();

    public:
        // Setup
        static void start(const CreateInfo& inCreateInfo);

        // Telemetry
        static const Telemetry& getTelemetry();

        // Game
        static bool hasController();
        static Controller* getController();
        static void setController(Controller* inController);
        static void watchController(
            std::function<void (Controller*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

        static bool hasLevel();
        static Level* getLevel();
        static void setLevel(Level* inLevel);
        static void watchLevel(
            std::function<void (Level*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

        static bool hasCamera();
        static CameraComponent* getCamera();
        static void setCamera(CameraComponent* inCamera);
        static void watchCamera(
            std::function<void (CameraComponent*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

        // Render
        static bool hasWindow();
        static Window* getWindow();

        static bool hasRenderer();
        static Renderer* getRenderer();
        template<class T>
        static T* getRenderer()
        {
            return static_cast<T*>(getRenderer());
        }

    public:
        // Lifecycle
        void init(const CreateInfo& inCreateInfo);
        void run();

    private:
        // Setup
        void initWindow(const Window::CreateInfo& inWindowCreateInfo);
        void initCamera();
        void initRenderer(Renderer::Type inRendererType);
        void initLayers();

        // Lifecycle
        void onEvent(const SDL_Event& inEvent);
        void onRender();

    public:
        // Telemetry
        Telemetry                                     telemetry;

        // Game
        Controller*                                   controller;
        std::unique_ptr<Observable<Controller*>>      controllerObservable;

        Level*                                        level;
        std::unique_ptr<Observable<Level*>>           levelObservable;

        CameraComponent*                              defaultCamera;
        CameraComponent*                              camera;
        std::unique_ptr<Observable<CameraComponent*>> cameraObservable;

        // Render
        std::unique_ptr<Window>                       window;
        std::unique_ptr<Renderer>                     renderer;
    };
}