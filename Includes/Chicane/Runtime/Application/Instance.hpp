#pragma once

#include "Chicane/Grid/Component/View.hpp"
#include "Chicane/Runtime/Application/CreateInfo.hpp"
#include "Chicane/Runtime/Controller.hpp"
#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    namespace Application
    {
        using ControllerObservable   = Observable<Controller*>;
        using ControllerSubscription = Subscription<Controller*>;

        using SceneObservable   = Observable<Scene*>;
        using SceneSubscription = Subscription<Scene*>;

        using ViewObservable   = Observable<Grid::View*>;
        using ViewSubscription = Subscription<Grid::View*>;

        class CHICANE_RUNTIME Instance
        {
        public:
            Instance();

        public:
            // Lifecycle
            void init(const CreateInfo& inCreateInfo);

            // Telemetry
            const Telemetry& getTelemetry() const;

            // Game
            bool hasController() const;
            Controller* getController() const;
            void setController(Controller* inController);
            ControllerSubscription watchController(
                ControllerSubscription::NextCallback inNext,
                ControllerSubscription::ErrorCallback inError = nullptr,
                ControllerSubscription::CompleteCallback inComplete = nullptr
            );

            bool hasScene() const;
            Scene* getScene() const;
            void setScene(Scene* inScene);
            SceneSubscription watchScene(
                SceneSubscription::NextCallback inNext,
                SceneSubscription::ErrorCallback inError = nullptr,
                SceneSubscription::CompleteCallback inComplete = nullptr
            );

            // UI
            bool hasView() const;
            Grid::View* getView() const;
            void setView(Grid::View* inView);
            ViewSubscription watchView(
                ViewSubscription::NextCallback inNext,
                ViewSubscription::ErrorCallback inError = nullptr,
                ViewSubscription::CompleteCallback inComplete = nullptr
            );

            // Render
            bool hasWindow() const;
            template<class T = Window>
            T* getWindow() const
            {
                if (!hasWindow())
                {
                    return nullptr;
                }

                if (typeid(T) == typeid(Window))
                {
                    return m_window.get();
                }

                return static_cast<T*>(m_window.get());
            }

            bool hasRenderer() const;
            void setRenderer(WindowRenderer inType);
            template<class T = Renderer::Instance>
            T* getRenderer() const
            {
                if (!hasRenderer())
                {
                    return nullptr;
                }

                if (typeid(T) == typeid(Renderer::Instance))
                {
                    return m_renderer.get();
                }

                return static_cast<T*>(m_renderer.get());
            }

        private:
            // Setup
            void initWindow();
            void initRenderer();
            void initAssets(const String& inPath);

            // Lifecycle
            void render();

        private:
            // Runtime
            Telemetry                           m_telemetry;

            Controller*                         m_controller;
            ControllerObservable                m_controllerObservable;

            Scene*                              m_scene;
            SceneObservable                     m_sceneObservable;

            // Grid
            Grid::View*                         m_view;
            ViewObservable                      m_viewObservable;

            // Window
            WindowCreateInfo                  m_windowInfo;
            std::unique_ptr<Window>   m_window;

            // Renderer
            Renderer::CreateInfo                m_rendererInfo;
            std::unique_ptr<Renderer::Instance> m_renderer;
        };
    }
}