#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    class Application
    {
    public:
        struct CreateInfo
        {
        public:
            Window::CreateInfo     window   = {};
            Renderer::Type         renderer = Renderer::Type::Undefined;

            // Callbacks
            std::function<void ()> onSetup  = nullptr;
        };

    public:
        // Setup
        static void run(const CreateInfo& inCreateInfo);
        static void stop();

        // Telemetry
        static const Telemetry& getTelemetry();

        // Game
        static bool hasController();
        static Controller* getController();
        static void setController(Controller* inController);
        static void watchController(
            std::function<void (Controller*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        static bool hasLevel();
        static Level* getLevel();
        static void setLevel(Level* inLevel);
        static void watchLevel(
            std::function<void (Level*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        static bool hasCamera();
        static CameraComponent* getCamera();
        static void setCamera(CameraComponent* inCamera);
        static void watchCamera(
            std::function<void (CameraComponent*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // UI
        static bool hasView();
        static void addView(Grid::View* inView);
        static void setView(const std::string& inId);
        static Grid::View* getView(const std::string& inId = "");
        static void watchView(
            std::function<void (Grid::View*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // Render
        static bool hasWindow();
        static Window* getWindow();

        static bool hasRenderer();
        static Renderer* getRenderer();
        template<class T>
        static T* getRenderer()
        {
            if (!hasRenderer())
            {
                return nullptr;
            }

            return static_cast<T*>(getRenderer());
        }

    public:
        Application();

    public:
        // Lifecycle
        void setup(const CreateInfo& inCreateInfo);
        void run();

    private:
        // Setup
        void initWindow(const Window::CreateInfo& inWindowCreateInfo);
        void initRenderer(Renderer::Type inRendererType);
        void initDefaultController();
        void initDefaultLevel();
        void initDefaultCamera();
        void initDefaultLayers();
        void initEvents();

        // Lifecycle
        void onEvent(const SDL_Event& inEvent);
        void onRender();

    public:
        // Telemetry
        Telemetry                                     telemetry;

        // Game
        std::unique_ptr<Controller>                   defaultController;
        Controller*                                   controller;
        std::unique_ptr<Observable<Controller*>>      controllerObservable;

        std::unique_ptr<Level>                        defaultLevel;
        Level*                                        level;
        std::unique_ptr<Observable<Level*>>           levelObservable;

        CameraComponent*                              defaultCamera;
        CameraComponent*                              camera;
        std::unique_ptr<Observable<CameraComponent*>> cameraObservable;

        // UI
        std::unordered_map<std::string, Grid::View*>  views;
        Grid::View*                                   view;
        std::unique_ptr<Observable<Grid::View*>>      viewObservable;

        // Render
        std::unique_ptr<Window>                       window;
        std::unique_ptr<Renderer>                     renderer;
    };
}