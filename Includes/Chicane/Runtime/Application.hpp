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

        // Lifecycle
        void CHICANE_RUNTIME run(const ApplicationCreateInfo& inCreateInfo);

        // Telemetry
        const CHICANE_RUNTIME Telemetry& getTelemetry();

        // Game
        bool CHICANE_RUNTIME hasController();
        Controller* getController();
        void CHICANE_RUNTIME setController(Controller* inController);
        ControllerSubscription CHICANE_RUNTIME watchController(
            ControllerSubscription::NextCallback inNext,
            ControllerSubscription::ErrorCallback inError = nullptr,
            ControllerSubscription::CompleteCallback inComplete = nullptr
        );

        bool CHICANE_RUNTIME hasScene();
        Scene* getScene();
        void CHICANE_RUNTIME setScene(Scene* inScene);
        SceneSubscription CHICANE_RUNTIME watchScene(
            SceneSubscription::NextCallback inNext,
            SceneSubscription::ErrorCallback inError = nullptr,
            SceneSubscription::CompleteCallback inComplete = nullptr
        );

        // UI
        bool CHICANE_RUNTIME hasView();
        Grid::View* getView();
        void CHICANE_RUNTIME setView(Grid::View* inView);
        ViewSubscription CHICANE_RUNTIME watchView(
            ViewSubscription::NextCallback inNext,
            ViewSubscription::ErrorCallback inError = nullptr,
            ViewSubscription::CompleteCallback inComplete = nullptr
        );

        // Render
        bool CHICANE_RUNTIME hasWindow();
        Window* getWindow();
        template<class T>
        T* getWindow()
        {
            if (!hasWindow())
            {
                return nullptr;
            }

            return static_cast<T*>(getWindow());
        }

        bool CHICANE_RUNTIME hasRenderer();
        void CHICANE_RUNTIME setRenderer(WindowRenderer inType);
        Renderer* getRenderer();
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