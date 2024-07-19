#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Grid.hpp"
#include "Runtime/Game/Controller.hpp"
#include "Runtime/Game/Level.hpp"
#include "Runtime/Renderer/Camera.hpp"

namespace Chicane
{
    class Window;

    namespace State
    {
        // Game
        bool hasCamera();
        Camera* getCamera();
        void setCamera(Camera* inCamera);
        void subscribeToCamera(
            std::function<void (Camera*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback,
            std::function<void ()> inCompleteCallback
        );

        bool hasController();
        Controller* getController();
        void setController(Controller* inController);

        bool hasLevel();
        Level* getLevel();
        void setLevel(Level* inLevel);
        void subscribeToLevel(
            std::function<void (Level*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback,
            std::function<void ()> inCompleteCallback
        );

        // Window
        Window* getWindow();
        void setWindow(Window* inWindow);

        Vec2 getResolution();
        void setResolution(const Vec2& inResolution);

        Telemetry getTelemetry();
    }
}