#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Game/Controller.hpp"
#include "Runtime/Renderer/Camera.hpp"

namespace Chicane
{ 
    namespace State
    {
        void setCamera(Camera* inCamera);
        Camera* getCamera();

        void setController(Controller* inController);
        Controller* getController();

        void setStats(const Stats& inStats);
        Stats getStats();

        void setResolution(const Vec2& inResolution);
        Vec2 getResolution();
    }
}