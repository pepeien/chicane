#pragma once

#include "Chicane/Runtime/Application/CreateInfo.hpp"
#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Game.hpp"
#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    namespace Application
    {
        using ControllerObservable   = Observable<Controller*>;
        using ControllerSubscription = Subscription<Controller*>;

        using LevelObservable   = Observable<Level*>;
        using LevelSubscription = Subscription<Level*>;

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

            bool hasLevel() const;
            Level* getLevel() const;
            void setLevel(Level* inLevel);
            LevelSubscription watchLevel(
                LevelSubscription::NextCallback inNext,
                LevelSubscription::ErrorCallback inError = nullptr,
                LevelSubscription::CompleteCallback inComplete = nullptr
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
            template<class T = Window::Instance>
            T* getWindow() const
            {
                if (!hasWindow())
                {
                    return nullptr;
                }

                if (typeid(T) == typeid(Window::Instance))
                {
                    return m_window.get();
                }

                return static_cast<T*>(m_window.get());
            }

            bool hasRenderer() const;
            void setRenderer(Window::Renderer inType);
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
            void initAssets(const std::string& inPath);

            // Lifecycle
            void render();

        private:
            // Telemetry
            Telemetry                           m_telemetry;

            // Game
            Controller*                         m_controller;
            ControllerObservable                m_controllerObservable;

            Level*                              m_level;
            LevelObservable                     m_levelObservable;

            // Grid
            Grid::View*                         m_view;
            ViewObservable                      m_viewObservable;

            // Window
            Window::CreateInfo                  m_windowInfo;
            std::unique_ptr<Window::Instance>   m_window;

            // Renderer
            Renderer::CreateInfo                m_rendererInfo;
            std::unique_ptr<Renderer::Instance> m_renderer;
        };
    }
}