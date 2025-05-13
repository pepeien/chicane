#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Runtime/Application/CreateInfo.hpp"
#include "Chicane/Runtime/Game.hpp"
#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    namespace Application
    {
        class CHICANE Instance
        {
        public:
            Instance();

        public:
            // Lifecycle
            void setup(const CreateInfo& inCreateInfo);
            void run();

            // Telemetry
            const Telemetry& getTelemetry();

            // Game
            bool hasController();
            Controller::Instance* getController();
            void setController(Controller::Instance* inController);
            Subscription<Controller::Instance*>* watchController(
                std::function<void (Controller::Instance*)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            );

            bool hasLevel();
            Level* getLevel();
            void setLevel(Level* inLevel);
            Subscription<Level*>* watchLevel(
                std::function<void (Level*)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            );

            // UI
            bool hasView();
            void addView(Grid::View* inView);
            void setView(const std::string& inId);
            Grid::View* getView(const std::string& inId = "");
            Subscription<Grid::View*>* watchView(
                std::function<void (Grid::View*)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            );

            // Render
            bool hasWindow();
            template<class T = Window::Instance>
            T* getWindow()
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

            bool hasRenderer();
            template<class T = Renderer::Instance>
            T* getRenderer()
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
            void initWindow(const Window::CreateInfo& inCreateInfo, Renderer::Type inRendererType);
            void initRenderer(const Renderer::CreateInfo& inCreateInfo);

            // Lifecycle
            void onEvent(const SDL_Event& inEvent);
            void onRender();

        private:
            // Telemetry
            Telemetry                                          m_telemetry;

            // Game
            Controller::Instance*                              m_controller;
            std::unique_ptr<Observable<Controller::Instance*>> m_controllerObservable;

            Level*                                             m_level;
            std::unique_ptr<Observable<Level*>>                m_levelObservable;

            // UI
            std::unordered_map<std::string, Grid::View*>       m_views;
            Grid::View*                                        m_view;
            std::unique_ptr<Observable<Grid::View*>>           m_viewObservable;

            // Render
            std::unique_ptr<Window::Instance>                  m_window;
            std::unique_ptr<Renderer::Instance>                m_renderer;
        };
    }
}