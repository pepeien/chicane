#include "Editor/View.hpp"

#include "Runtime/Core.hpp"
#include "Runtime/Game/State.hpp"

namespace Chicane
{
    namespace Editor
    {
        View::View()
            : Grid::View(
                "home",
                "./Content/View/Home.xml"
            )
        {
            addFunction(
                "getFPS",
                std::bind(&View::getFPS, this, std::placeholders::_1)
            );
            addFunction(
                "getFrametime",
                std::bind(&View::getFrametime, this, std::placeholders::_1)
            );
        }

        std::string View::getFPS(Grid::ComponentEvent inEvent)
        {
            std::string FPS = std::to_string(State::getTelemetry().framerate);

            return FPS + " FPS";
        }

        std::string View::getFrametime(Grid::ComponentEvent inEvent)
        {
            std::string frametime = std::to_string(State::getTelemetry().time);

            return std::string(frametime.begin(), frametime.end() - 5) + " ms";
        }
    }
}