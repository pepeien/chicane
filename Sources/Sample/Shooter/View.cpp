#include "View.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Game.hpp"
#include "Level.hpp"

View::View()
    : Chicane::Grid::View("Contents/Sample/Shooter/Views/Home.grid"),
      m_bDidPlayerWin(true),
      m_telemetry(Chicane::Application::getTelemetry()),
      m_uiFrameTime(Chicane::Reference::fromValue<const float>(&m_telemetry.delta)),
      m_uiFramesPerSecond(Chicane::Reference::fromValue<const std::uint32_t>(&m_telemetry.rate)),
      m_victoryVisibility(Chicane::Grid::Style::DISPLAY_TYPE_HIDDEN),
      m_uiVictoryVisibility(Chicane::Reference::fromValue<Chicane::String>(&m_victoryVisibility)),
      m_currentCamera("None"),
      m_uiCurrentCamera(Chicane::Reference::fromValue<Chicane::String>(&m_currentCamera)),
      m_crosshairDotVisibility(Chicane::Grid::Style::DISPLAY_TYPE_HIDDEN),
      m_uiCrosshairDotVisibility(
          Chicane::Reference::fromValue<Chicane::String>(&m_crosshairDotVisibility)
      ),
      m_crosshairSize(1.5f),
      m_uiCrosshairSize(Chicane::Reference::fromValue<float>(&m_crosshairSize)),
      m_crosshairThickness(0.25f),
      m_uiCrosshairThickness(Chicane::Reference::fromValue<float>(&m_crosshairThickness)),
      m_crosshairGap(0.05f),
      m_uiCrosshairGap(Chicane::Reference::fromValue<float>(&m_crosshairGap)),
      m_crosshairColorR(255),
      m_uiCrosshairColorR(Chicane::Reference::fromValue<std::uint8_t>(&m_crosshairColorR)),
      m_crosshairColorG(255),
      m_uiCrosshairColorG(Chicane::Reference::fromValue<std::uint8_t>(&m_crosshairColorG)),
      m_crosshairColorB(255),
      m_uiCrosshairColorB(Chicane::Reference::fromValue<std::uint8_t>(&m_crosshairColorB)),
      m_crosshairColorA(255),
      m_uiCrosshairColorA(Chicane::Reference::fromValue<std::uint8_t>(&m_crosshairColorA)),
      m_playerScore(0U),
      m_uiPlayerScore(Chicane::Reference::fromValue<std::uint32_t>(&m_playerScore))
{
    Game::watchScore([this](std::uint32_t inScore) {
        m_playerScore++;

        if (Game::didReachMaxScore())
        {
            m_bDidPlayerWin = true;

            m_victoryVisibility = Chicane::Grid::Style::DISPLAY_TYPE_FLEX;
        }
    });

    Chicane::Application::getScene<Level>()->watchActiveCamera(
        [this](Chicane::ACamera* inCamera) {
            if (inCamera == nullptr)
            {
                m_currentCamera = "First Person";

                return;
            }

            m_currentCamera = inCamera->getId();
        },
        [this](const Chicane::String& inError) { m_currentCamera = "None"; }
    );

    addReference("frameTime", &m_uiFrameTime);
    addReference("framesPerSecond", &m_uiFramesPerSecond);
    addReference("victoryDisplay", &m_uiVictoryVisibility);
    addReference("currentCamera", &m_uiCurrentCamera);
    addReference("crosshairDotDisplay", &m_uiCrosshairDotVisibility);
    addReference("crosshairSize", &m_uiCrosshairSize);
    addReference("crosshairThickness", &m_uiCrosshairThickness);
    addReference("crosshairGap", &m_uiCrosshairGap);
    addReference("crosshairColorR", &m_uiCrosshairColorR);
    addReference("crosshairColorG", &m_uiCrosshairColorG);
    addReference("crosshairColorB", &m_uiCrosshairColorB);
    addReference("crosshairColorA", &m_uiCrosshairColorA);
    addReference("playerScore", &m_uiPlayerScore);
}