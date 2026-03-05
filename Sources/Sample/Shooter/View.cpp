#include "View.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Game.hpp"
#include "Level.hpp"

static int switcher = 1;

View::View()
    : Chicane::Grid::View("Contents/Sample/Shooter/Views/Home.grid"),
      m_telemetry(Chicane::Application::getInstance().getTelemetry()),
      m_bDidPlayerWin(false),
      m_currentCamera("None"),
      m_crosshairDotVisibility(Chicane::Grid::Style::DISPLAY_TYPE_BLOCK),
      m_crosshairSize(2.0f),
      m_crosshairThickness(1.0f),
      m_crosshairGap(1.0f),
      m_crosshairColorR(255),
      m_crosshairColorG(255),
      m_crosshairColorB(255),
      m_crosshairColorA(1.0f),
      m_playerScore(0U),
      m_maxScore(0U),
      u_DidPlayerWin(Chicane::Reference::fromValue<const bool>(&m_bDidPlayerWin)),
      u_rendererFrameTime(Chicane::Reference::fromValue<const float>(&m_telemetry.renderer.delta)),
      u_rendererFramesPerSecond(Chicane::Reference::fromValue<const std::uint32_t>(&m_telemetry.renderer.rate)),
      u_sceneFrameTime(Chicane::Reference::fromValue<const float>(&m_telemetry.scene.delta)),
      u_sceneFramesPerSecond(Chicane::Reference::fromValue<const std::uint32_t>(&m_telemetry.scene.rate)),
      u_uiFrameTime(Chicane::Reference::fromValue<const float>(&m_telemetry.ui.delta)),
      u_uiFramesPerSecond(Chicane::Reference::fromValue<const std::uint32_t>(&m_telemetry.ui.rate)),
      u_currentCamera(Chicane::Reference::fromValue<Chicane::String>(&m_currentCamera)),
      u_crosshairDotVisibility(Chicane::Reference::fromValue<Chicane::String>(&m_crosshairDotVisibility)),
      u_crosshairSize(Chicane::Reference::fromValue<float>(&m_crosshairSize)),
      u_crosshairThickness(Chicane::Reference::fromValue<float>(&m_crosshairThickness)),
      u_crosshairGap(Chicane::Reference::fromValue<float>(&m_crosshairGap)),
      u_crosshairColorR(Chicane::Reference::fromValue<std::uint8_t>(&m_crosshairColorR)),
      u_crosshairColorG(Chicane::Reference::fromValue<std::uint8_t>(&m_crosshairColorG)),
      u_crosshairColorB(Chicane::Reference::fromValue<std::uint8_t>(&m_crosshairColorB)),
      u_crosshairColorA(Chicane::Reference::fromValue<float>(&m_crosshairColorA)),
      u_playerScore(Chicane::Reference::fromValue<std::uint32_t>(&m_playerScore)),
      u_maxScore(Chicane::Reference::fromValue<std::uint32_t>(&m_maxScore))
{
    m_maxScore = Game::getMaxScore();

    Game::watchScore(
        [this](std::uint32_t inScore)
        {
            m_playerScore++;

            if (Game::didReachMaxScore())
            {
                m_bDidPlayerWin = true;
            }
        }
    );

    Chicane::Application::getInstance().getScene<Level>()->watchActiveCamera(
        [this](Chicane::ACamera* inCamera)
        {
            if (inCamera == nullptr)
            {
                m_currentCamera = "First Person";

                return;
            }

            m_currentCamera = inCamera->getId();
        },
        [this](const Chicane::String& inError) { m_currentCamera = "None"; }
    );

    addReference("didPlayerWin", &u_DidPlayerWin);

    addReference("rendererFrameTime", &u_rendererFrameTime);
    addReference("rendererFramesPerSecond", &u_rendererFramesPerSecond);
    addReference("sceneFrameTime", &u_sceneFrameTime);
    addReference("sceneFramesPerSecond", &u_sceneFramesPerSecond);
    addReference("uiFrameTime", &u_uiFrameTime);
    addReference("uiFramesPerSecond", &u_uiFramesPerSecond);

    addReference("currentCamera", &u_currentCamera);

    addReference("crosshairDotDisplay", &u_crosshairDotVisibility);
    addReference("crosshairSize", &u_crosshairSize);
    addReference("crosshairThickness", &u_crosshairThickness);
    addReference("crosshairGap", &u_crosshairGap);
    addReference("crosshairColorR", &u_crosshairColorR);
    addReference("crosshairColorG", &u_crosshairColorG);
    addReference("crosshairColorB", &u_crosshairColorB);
    addReference("crosshairColorA", &u_crosshairColorA);

    addReference("playerScore", &u_playerScore);

    addReference("maxScore", &u_maxScore);
}