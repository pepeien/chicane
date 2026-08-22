#include "Sample/Shooter/UI/View/Home.reflected.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Sample/Shooter/Game.hpp"
#include "Sample/Shooter/Scene.hpp"
#include "Sample/Shooter/UI/Component/Telemetry.hpp"

HomeView::HomeView()
    : Chicane::Grid::View(),
      bDidPlayerWin(false),
      crosshairDotVisibility(Chicane::Grid::Style::DISPLAY_TYPE_BLOCK),
      crosshairSize(2.0f),
      crosshairThickness(1.0f),
      crosshairGap(1.0f),
      crosshairColorR(255),
      crosshairColorG(255),
      crosshairColorB(255),
      crosshairColorA(1.0f),
      playerScore(0U),
      maxScore(0U)
{
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
