#include "Runtime/Grid/Views/Gameplay.hpp"

#include "Runtime/Core.hpp"
#include "Runtime/Game.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace GameplayView
        {
            void onFPSCounterTick(pugi::xml_node& outNode)
            {
                outNode
                .first_child()
                .set_value(
                    std::to_string(State::getStats().framerate).c_str()
                );
            }

            void onFrametimeTick(pugi::xml_node& outNode)
            {
                std::string frametime = std::to_string(State::getStats().time);

                outNode
                .first_child()
                .set_value(
                    (std::string(frametime.begin(), frametime.end() - 5) + " ms").c_str()
                );
            }
        }
    }
}