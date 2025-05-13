#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Runtime/Application/CreateInfo.hpp"
#include "Chicane/Runtime/Application/Instance.hpp"
#include "Chicane/Runtime/Game.hpp"
#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    namespace Application
    {
        // Setup
        CHICANE void run(const CreateInfo& inCreateInfo);
        CHICANE void stop();

        // Telemetry
        CHICANE const Telemetry& getTelemetry();

        // Game
        CHICANE bool hasController();
        CHICANE Controller::Instance* getController();
        CHICANE void setController(Controller::Instance* inController);
        CHICANE Subscription<Controller::Instance*>* watchController(
            std::function<void (Controller::Instance*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        CHICANE bool hasLevel();
        CHICANE Level* getLevel();
        CHICANE void setLevel(Level* inLevel);
        CHICANE Subscription<Level*>* watchLevel(
            std::function<void (Level*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // UI
        CHICANE bool hasView();
        CHICANE void addView(Grid::View* inView);
        CHICANE void setView(const std::string& inId);
        CHICANE Grid::View* getView(const std::string& inId = "");
        CHICANE Subscription<Grid::View*>* watchView(
            std::function<void (Grid::View*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        // Render
        CHICANE bool hasWindow();
        CHICANE Window::Instance* getWindow();
        template<class T>
        T* getWindow()
        {
            if (!hasWindow())
            {
                return nullptr;
            }

            return static_cast<T*>(getWindow());
        }

        CHICANE bool hasRenderer();
        CHICANE Renderer::Instance* getRenderer();
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