#pragma once

#include "Chicane/Runtime/Application/CreateInfo.hpp"
#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Game.hpp"
#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    namespace Application
    {
        class CHICANE_RUNTIME Instance
        {
        public:
            Instance();

        public:
            // Lifecycle
            void setup(const CreateInfo& inCreateInfo);
            void run();

            // Telemetry
            const Telemetry& getTelemetry() const;

            // Game
            bool hasController() const;
            Controller* getController() const;
            void setController(Controller* inController);
            Subscription<Controller*>* watchController(
                std::function<void (Controller*)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            ) const;

            bool hasLevel() const;
            Level* getLevel() const;
            void setLevel(Level* inLevel);
            Subscription<Level*>* watchLevel(
                std::function<void (Level*)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            ) const;

            // UI
            bool hasView() const;
            Grid::View* getView() const;
            void setView(Grid::View* inView);
            Subscription<Grid::View*>* watchView(
                std::function<void (Grid::View*)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            ) const;

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
            void initWindow(const Window::CreateInfo& inCreateInfo, Renderer::Type inRendererType);
            void initRenderer(const Renderer::CreateInfo& inCreateInfo);
            void initAssets(const std::string& inPath);

            // Lifecycle
            void onEvent(const SDL_Event& inEvent);
            void onRender();

        private:
            // Telemetry
            Telemetry                                m_telemetry;

            // Game
            Controller*                              m_controller;
            std::unique_ptr<Observable<Controller*>> m_controllerObservable;

            Level*                                   m_level;
            std::unique_ptr<Observable<Level*>>      m_levelObservable;

            // UI
            Grid::View*                              m_view;
            std::unique_ptr<Observable<Grid::View*>> m_viewObservable;

            // Render
            std::unique_ptr<Window::Instance>        m_window;
            std::unique_ptr<Renderer::Instance>      m_renderer;
        };
    }
}