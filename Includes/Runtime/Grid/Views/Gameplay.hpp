#pragma once

#include "Runtime/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace GameplayView
        {
            const std::string VIEW_ID = "gameplay-hud";

            void onFPSCounterTick(pugi::xml_node& outNode);
            void onFrametimeTick(pugi::xml_node& outNode);

            const ComponentCallbackMap Callbacks = {
                { "onFPSCounterTick", &onFPSCounterTick },
                { "onFrametimeTick", &onFrametimeTick }
            };
        }
    }
}