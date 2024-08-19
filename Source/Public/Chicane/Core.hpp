#pragma once

#include "Chicane/Core/Allocator.hpp"
#include "Chicane/Core/Box.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Helper.hpp"
#include "Chicane/Core/Layer.hpp"
#include "Chicane/Core/Layers/Skybox.hpp"
#include "Chicane/Core/Layers/Level.hpp"
#include "Chicane/Core/Layers/UI.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Thread.hpp"
#include "Chicane/Core/Utils.hpp"
#include "Chicane/Core/Window.hpp"

namespace Chicane
{
    class Camera;
    class Controller;
    class Level;

    struct Telemetry;

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
    void addActor(Actor* inActor);

    // Window
    Window* getWindow();
    void setWindow(Window* inWindow);

    bool isWindowFocused();
    void setWindowFocus(bool inIsFocused);

    Vec<int>::Two getCursorPosition();

    Vec<int>::Two getResolution();
    void setResolution(const Vec<int>::Two& inResolution);

    Telemetry getTelemetry();
}