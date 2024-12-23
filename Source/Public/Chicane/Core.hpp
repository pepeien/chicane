#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Layer.hpp"
#include "Chicane/Core/Layers/Skybox.hpp"
#include "Chicane/Core/Layers/Level.hpp"
#include "Chicane/Core/Layers/UI.hpp"
#include "Chicane/Core/Loader.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Utils.hpp"
#include "Chicane/Core/Window.hpp"

namespace Chicane
{
    class Actor;
    class ActorComponent;
    class CameraComponent;
    class Controller;
    class Level;

    struct Telemetry;

    // Game
    bool hasActiveCamera();
    CameraComponent* getActiveCamera();
    void setActiveCamera(CameraComponent* inCamera);
    void watchActiveCamera(
        std::function<void (CameraComponent*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback = nullptr,
        std::function<void ()> inCompleteCallback = nullptr
    );

    bool hasActiveController();
    Controller* getActiveController();
    void setActiveController(Controller* inController);

    bool hasActiveLevel();
    Level* getActiveLevel();
    void setActiveLevel(Level* inLevel);
    void watchActiveLevel(
        std::function<void (Level*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback = nullptr,
        std::function<void ()> inCompleteCallback = nullptr
    );

    void addActor(Actor* inActor);
    std::vector<Actor*> getActors();

    void addComponent(ActorComponent* inComponent);
    void removeComponent(ActorComponent* inComponent);
    std::vector<ActorComponent*> getComponents();

    // Window
    Window* getWindow();
    void setWindow(Window* inWindow);

    bool isWindowFocused();
    void setWindowFocus(bool bInIsFocused);

    Vec<2, float> getCursorPosition();

    const Vec<2, int>& getResolution();
    void setResolution(const Vec<2, int>& inResolution);

    void setViewport(
        const Vec<2, std::uint32_t>& inSize,
        const Vec<2, float>& inPosition = Vec2Zero
    );

    const Telemetry& getTelemetry();
}