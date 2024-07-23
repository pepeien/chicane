#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Game/Controller.hpp"
#include "Chicane/Game/Level.hpp"
#include "Chicane/Renderer/Camera.hpp"

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

        Vec<int>::Two getResolution();
        void setResolution(const Vec<int>::Two& inResolution);

        Telemetry getTelemetry();
    }
}