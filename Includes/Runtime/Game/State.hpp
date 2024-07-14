#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Game/Controller.hpp"
#include "Runtime/Game/Level.hpp"
#include "Runtime/Grid.hpp"
#include "Runtime/Renderer/Camera.hpp"

namespace Chicane
{
    class Window;

    namespace State
    {
        Window* getWindow();
        void setWindow(Window* inWindow);

        Controller* getController();
        void setController(Controller* inController);

        Telemetry getTelemetry();

        Vec2 getResolution();
        void setResolution(const Vec2& inResolution);

        Level* getLevel();
        void setLevel(Level* inLevel);

        void addModel(const Box::Instance& inBox);
    }
}