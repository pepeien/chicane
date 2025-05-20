#pragma once

#include "Chicane/Runtime/Application/CreateInfo.hpp"
#include "Chicane/Runtime/Application/Instance.hpp"
#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Game.hpp"
#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    namespace Application
    {
        // Setup
        CHICANE_RUNTIME void run(const CreateInfo& inCreateInfo);
        CHICANE_RUNTIME void stop();

        // Telemetry
        CHICANE_RUNTIME const Telemetry& getTelemetry();

        // Game
        CHICANE_RUNTIME bool hasController();
        CHICANE_RUNTIME Controller* getController();
        CHICANE_RUNTIME void setController(Controller* inController);
        CHICANE_RUNTIME Subscription<Controller*>* watchController(
            std::function<void (Controller*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        CHICANE_RUNTIME bool hasLevel();
        CHICANE_RUNTIME Level* getLevel();
        CHICANE_RUNTIME void setLevel(Level* inLevel);
        CHICANE_RUNTIME Subscription<Level*>* watchLevel(
            std::function<void (Level*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // UI
        CHICANE_RUNTIME bool hasView();
        CHICANE_RUNTIME Grid::View* getView();
        CHICANE_RUNTIME void setView(Grid::View* inView);
        CHICANE_RUNTIME Subscription<Grid::View*>* watchView(
            std::function<void (Grid::View*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // Render
        CHICANE_RUNTIME bool hasWindow();
        CHICANE_RUNTIME Window::Instance* getWindow();
        template<class T>
        T* getWindow()
        {
            if (!hasWindow())
            {
                return nullptr;
            }

            return static_cast<T*>(getWindow());
        }

        CHICANE_RUNTIME bool hasRenderer();
        CHICANE_RUNTIME Renderer::Instance* getRenderer();
        template<class T>
        T* getRenderer()
        {
            if (!hasRenderer())
            {
                return nullptr;
            }

            return static_cast<T*>(getRenderer());
        }
    }
}