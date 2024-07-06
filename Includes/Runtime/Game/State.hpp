#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Game/Controller.hpp"
#include "Runtime/Game/Level.hpp"
#include "Runtime/Grid.hpp"
#include "Runtime/Renderer/Camera.hpp"

namespace Chicane
{ 
    namespace State
    {
        void setCamera(Camera* inCamera);
        Camera* getCamera();

        void setController(Controller* inController);
        Controller* getController();

        void setTelemetry(const Telemetry& inTelemtry);
        Telemetry getTelemetry();

        void setResolution(const Vec2& inResolution);
        Vec2 getResolution();

        void setLevel(Level* inLevel);
        Level* getLevel();

        void addModel(const Box::Instance& inBox);
    }
}