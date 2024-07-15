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
            addFunction(
                "getActorDetails",
                std::bind(&View::getActorDetails, this, std::placeholders::_1)
            );
        }

        std::uint64_t View::getFPS(Grid::ComponentEvent inEvent)
        {
            return State::getTelemetry().framerate;
        }

        std::string View::getFrametime(Grid::ComponentEvent inEvent)
        {
            std::string frametime = std::to_string(State::getTelemetry().time);

            return std::string(frametime.begin(), frametime.end() - 5);
        }

        int View::getActorDetails(Grid::ComponentEvent inEvent)
        {
            Actor* actor = std::any_cast<Actor*>(inEvent.values[0]);

            Grid::TextComponent::compileRaw(actor->getModel().name);

            return 0;
        }
    }
}