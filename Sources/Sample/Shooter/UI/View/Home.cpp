#include "Sample/Shooter/UI/View/Home.reflected.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Sample/Shooter/Game.hpp"
#include "Sample/Shooter/Scene.hpp"
#include "Sample/Shooter/UI/Component/Crosshair.hpp"
#include "Sample/Shooter/UI/Component/Telemetry.hpp"

HomeView::HomeView()
    : Chicane::Grid::View(),
      bDidPlayerWin(false),
      playerScore(0U),
      maxScore(0U),
      input(""),
      crosshair()
{
    import <Crosshair>();
    import <Telemetry>();
    load("Assets/Sample/Shooter/UI/Views/Home.grid", "Assets/Sample/Shooter/UI/Views/Home.decal");

    maxScore = Game::getMaxScore();

    Game::watchScore(
        [this](std::uint32_t inScore)
        {
            playerScore++;

            if (Game::didReachMaxScore())
            {
                bDidPlayerWin = true;
            }
        }
    );
}
