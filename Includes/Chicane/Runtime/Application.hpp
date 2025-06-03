#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Application/CreateInfo.hpp"
#include "Chicane/Runtime/Application/Instance.hpp"

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
        CHICANE_RUNTIME ControllerSubscription watchController(
            ControllerSubscription::NextCallback inNext,
            ControllerSubscription::ErrorCallback inError = nullptr,
            ControllerSubscription::CompleteCallback inComplete = nullptr
        );

        CHICANE_RUNTIME bool hasLevel();
        CHICANE_RUNTIME Level* getLevel();
        CHICANE_RUNTIME void setLevel(Level* inLevel);
        CHICANE_RUNTIME LevelSubscription watchLevel(
            LevelSubscription::NextCallback inNext,
            LevelSubscription::ErrorCallback inError = nullptr,
            LevelSubscription::CompleteCallback inComplete = nullptr
        );

        // UI
        CHICANE_RUNTIME bool hasView();
        CHICANE_RUNTIME Grid::View* getView();
        CHICANE_RUNTIME void setView(Grid::View* inView);
        CHICANE_RUNTIME ViewSubscription watchView(
            ViewSubscription::NextCallback inNext,
            ViewSubscription::ErrorCallback inError = nullptr,
            ViewSubscription::CompleteCallback inComplete = nullptr
        );

        // Render
        CHICANE_RUNTIME bool hasWindow();
        CHICANE_RUNTIME Window::Instance* getWindow();

        CHICANE_RUNTIME bool hasRenderer();
        CHICANE_RUNTIME void setRenderer(Window::Renderer inType);
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