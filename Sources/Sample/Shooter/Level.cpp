#include "Sample/Shooter/Level.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene/Actor/Camera.hpp>
#include <Chicane/Runtime/Scene/Actor/Sky.hpp>

#include "Actor/Apple.hpp"
#include "Actor/Character.hpp"
#include "Actor/Structure.hpp"
#include "Actor/Sun.hpp"

#include "Game.hpp"

static constexpr inline const std::uint32_t APPLE_DEPTH_COUNT  = 2;
static constexpr inline const std::uint32_t APPLE_COLUMN_COUNT = 5;
static constexpr inline const std::uint32_t APPLE_ROW_COUNT    = 2;
static constexpr inline const float         APPLE_STEP         = 20.0f;

Level::Level()
    : Chicane::Scene(),
      m_leftCamera(nullptr),
      m_centerCamera(nullptr),
      m_rightCamera(nullptr)
{}

void Level::onLoad()
{
    spawnSky();
    spawnLights();
    spawnCameras();
    spawnStructures();
    spawnCharacter();
    spawnApples();
}

void Level::activateLeftCamera()
{
    m_leftCamera->activate();

    m_centerCamera->deactivate();
    m_rightCamera->deactivate();
}

void Level::activateCenterCamera()
{
    m_centerCamera->activate();

    m_leftCamera->deactivate();
    m_rightCamera->deactivate();
}

void Level::activateRightCamera()
{
    m_rightCamera->activate();

    m_leftCamera->deactivate();
    m_centerCamera->deactivate();
}

void Level::disableCameras()
{
    m_leftCamera->deactivate();
    m_centerCamera->deactivate();
    m_rightCamera->deactivate();
}

void Level::spawnSky()
{
    createActor<Chicane::ASky>()->setSky(Chicane::Box::load<Chicane::Box::Sky>("Contents/Engine/Skies/Debug.bsky"));
}

void Level::spawnLights()
{
    createActor<Sun>();
}

void Level::spawnCameras()
{
    const Chicane::Vec3 position(300.0f, 300.0f, 100.0f);

    m_leftCamera = createActor<Chicane::ACamera>();
    m_leftCamera->setId("Left");
    m_leftCamera->setAbsoluteTranslation(-position.x, -position.y, position.z);
    m_leftCamera->lookAt(Chicane::Vec3::Zero);

    m_centerCamera = createActor<Chicane::ACamera>();
    m_centerCamera->setId("Center");
    m_centerCamera->setAbsoluteTranslation(0.0f, -position.y, position.z);
    m_centerCamera->lookAt(Chicane::Vec3::Zero);

    m_rightCamera = createActor<Chicane::ACamera>();
    m_rightCamera->setId("Right");
    m_rightCamera->setAbsoluteTranslation(position.x, -position.y, position.z);
    m_rightCamera->lookAt(Chicane::Vec3::Zero);
}

void Level::spawnStructures()
{
    createActor<Strcuture>()->setAbsoluteScale(500.0f, 500.0f, 0.5);
}

void Level::spawnCharacter()
{
    Chicane::Application::getInstance().getController()->attachTo(createActor<Character>());
}

void Level::spawnApples()
{
    const Chicane::Vec3 startPosition(
        -(APPLE_COLUMN_COUNT * APPLE_STEP) * 0.5f,
        (APPLE_DEPTH_COUNT * APPLE_STEP) * 0.5f,
        (APPLE_ROW_COUNT * APPLE_STEP) * 0.5f
    );
    Chicane::Vec3 position = startPosition;

    Game::setMaxScore(APPLE_ROW_COUNT * APPLE_COLUMN_COUNT * APPLE_DEPTH_COUNT);

    for (std::uint32_t depth = 0; depth < APPLE_DEPTH_COUNT; depth++)
    {
        for (std::uint32_t row = 0; row < APPLE_ROW_COUNT; row++)
        {
            for (std::uint32_t column = 0; column < APPLE_COLUMN_COUNT; column++)
            {
                createActor<Apple>()->setInitialPosition(position);

                position.x += APPLE_STEP;
            }

            position.x = startPosition.x;
            position.z += APPLE_STEP;
        }

        position.y += APPLE_STEP;
        position.z = startPosition.z;
    }
}
