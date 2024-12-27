#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Layer.hpp"
#include "Chicane/Core/Layer/SkyboxLayer.hpp"
#include "Chicane/Core/Layer/LevelLayer.hpp"
#include "Chicane/Core/Layer/UILayer.hpp"
#include "Chicane/Core/Loader.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Utils.hpp"
#include "Chicane/Core/Window.hpp"

namespace Chicane
{
    class Actor;
    class Component;
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

    void addComponent(Component* inComponent);
    void removeComponent(Component* inComponent);
    std::vector<Component*> getComponents();

    // Window
    Window::Instance* getWindow();
    void setWindow(Window::Instance* inWindow);

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