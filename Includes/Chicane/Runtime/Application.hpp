#pragma once

#include "Chicane/Core/Telemetry.hpp"
#include "Chicane/Core/Window.hpp"

#include "Chicane/Grid/Component/View.hpp"

#include "Chicane/Renderer/Instance.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Application/CreateInfo.hpp"
#include "Chicane/Runtime/Controller.hpp"
#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    namespace Application
    {
        using ControllerObservable   = EventObservable<Controller*>;
        using ControllerSubscription = EventSubscription<Controller*>;

        using SceneObservable        = EventObservable<Scene*>;
        using SceneSubscription      = EventSubscription<Scene*>;

        using ViewObservable         = EventObservable<Grid::View*>;
        using ViewSubscription       = EventSubscription<Grid::View*>;

        // Lifecycle
        CHICANE_RUNTIME void run(const ApplicationCreateInfo& inCreateInfo);

        // Telemetry
        CHICANE_RUNTIME const Telemetry& getTelemetry();

        // Game
        CHICANE_RUNTIME bool hasController();
        CHICANE_RUNTIME Controller* getController();
        CHICANE_RUNTIME void setController(Controller* inController);
        CHICANE_RUNTIME ControllerSubscription watchController(
            ControllerSubscription::NextCallback     inNext,
            ControllerSubscription::ErrorCallback    inError    = nullptr,
            ControllerSubscription::CompleteCallback inComplete = nullptr
        );

        CHICANE_RUNTIME bool hasScene();
        CHICANE_RUNTIME void setScene(Scene* inScene);
        CHICANE_RUNTIME Scene* getScene();
        template <class T>
        T* getScene()
        {
            if (!hasScene())
            {
                return nullptr;
            }

            return static_cast<T*>(getScene());
        }
        CHICANE_RUNTIME SceneSubscription watchScene(
            SceneSubscription::NextCallback     inNext,
            SceneSubscription::ErrorCallback    inError    = nullptr,
            SceneSubscription::CompleteCallback inComplete = nullptr
        );

        // UI
        CHICANE_RUNTIME bool hasView();
        CHICANE_RUNTIME Grid::View* getView();
        CHICANE_RUNTIME void setView(Grid::View* inView);
        CHICANE_RUNTIME ViewSubscription watchView(
            ViewSubscription::NextCallback     inNext,
            ViewSubscription::ErrorCallback    inError    = nullptr,
            ViewSubscription::CompleteCallback inComplete = nullptr
        );

        // Window
        CHICANE_RUNTIME bool hasWindow();
        CHICANE_RUNTIME Window* getWindow();

        // Renderer
        CHICANE_RUNTIME bool hasRenderer();
        CHICANE_RUNTIME Renderer::Instance* getRenderer();
    }
}