#include "Sample/Shooter/View.reflected.hpp"

#include <Chicane/Core/Log.hpp>

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Sample/Shooter/Game.hpp"
#include "Sample/Shooter/Level.hpp"

View::View()
    : Chicane::Grid::View("Contents/Sample/Shooter/Views/Home.grid"),
      telemetry(&Chicane::Application::getInstance().getTelemetry()),
      bDidPlayerWin(false),
      currentCamera("None"),
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

    Chicane::Application::getInstance().getScene<Level>()->watchActiveCamera(
        [this](Chicane::ACamera* inCamera)
        {
            if (inCamera == nullptr)
            {
                currentCamera = "First Person";

                return;
            }

            currentCamera = inCamera->getId();
        },
        [this](const Chicane::String& inError) { currentCamera = "None"; }
    );
}