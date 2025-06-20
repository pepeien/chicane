#pragma once

#include "Chicane/Grid/Component/View.hpp"
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

        CHICANE_RUNTIME bool hasScene();
        CHICANE_RUNTIME Scene* getScene();
        CHICANE_RUNTIME void setScene(Scene* inScene);
        CHICANE_RUNTIME SceneSubscription watchScene(
            SceneSubscription::NextCallback inNext,
            SceneSubscription::ErrorCallback inError = nullptr,
            SceneSubscription::CompleteCallback inComplete = nullptr
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
        CHICANE_RUNTIME Window* getWindow();

        CHICANE_RUNTIME bool hasRenderer();
        CHICANE_RUNTIME void setRenderer(WindowRenderer inType);
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