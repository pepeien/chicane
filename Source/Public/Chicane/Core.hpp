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
    bool hasController();
    Controller* getController();
    void setController(Controller* inController);

    bool hasLevel();
    Level* getLevel();
    void setLevel(Level* inLevel);
    void watchLevel(
        std::function<void (Level*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    );

    void addActor(Actor* inActor);

    bool hasCamera();
    Camera* getCamera();
    void setCamera(Camera* inCamera);
    void watchCamera(
        std::function<void (Camera*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback = nullptr,
        std::function<void ()> inCompleteCallback = nullptr
    );

    std::vector<ActorComponent*> getComponents();
    void addComponent(ActorComponent* inComponent);

    // Window
    Window* getWindow();
    void setWindow(Window* inWindow);

    bool isWindowFocused();
    void setWindowFocus(bool inIsFocused);

    Vec<2, int> getCursorPosition();

    Vec<2, int> getResolution();
    void setResolution(const Vec<2, int>& inResolution);

    void setViewport(
        const Vec<2, std::uint32_t>& inSize,
        const Vec<2, float>& inPosition = Vec<2, float>(0.0f)
    );

    Telemetry getTelemetry();
}