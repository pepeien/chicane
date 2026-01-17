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
        void CHICANE_RUNTIME run(const ApplicationCreateInfo& inCreateInfo);

        // Telemetry
        const CHICANE_RUNTIME Telemetry& getTelemetry();

        // Game
        bool CHICANE_RUNTIME hasController();
        Controller* getController();
        void CHICANE_RUNTIME setController(Controller* inController);
        ControllerSubscription CHICANE_RUNTIME watchController(
            ControllerSubscription::NextCallback     inNext,
            ControllerSubscription::ErrorCallback    inError    = nullptr,
            ControllerSubscription::CompleteCallback inComplete = nullptr
        );

        bool CHICANE_RUNTIME hasScene();
        void CHICANE_RUNTIME setScene(Scene* inScene);
        Scene* getScene();
        template <class T>
        T* getScene()
        {
            if (!hasScene())
            {
                return nullptr;
            }

            return static_cast<T*>(getScene());
        }
        SceneSubscription CHICANE_RUNTIME watchScene(
            SceneSubscription::NextCallback     inNext,
            SceneSubscription::ErrorCallback    inError    = nullptr,
            SceneSubscription::CompleteCallback inComplete = nullptr
        );

        // UI
        bool CHICANE_RUNTIME hasView();
        Grid::View* getView();
        void CHICANE_RUNTIME setView(Grid::View* inView);
        ViewSubscription CHICANE_RUNTIME watchView(
            ViewSubscription::NextCallback     inNext,
            ViewSubscription::ErrorCallback    inError    = nullptr,
            ViewSubscription::CompleteCallback inComplete = nullptr
        );

        // Window
        bool CHICANE_RUNTIME hasWindow();
        Window* getWindow();

        // Renderer
        bool CHICANE_RUNTIME hasRenderer();
        Renderer::Instance* getRenderer();
    }
}