#include "Level.hpp"

#include "Chicane/Runtime/Scene/Actor/Camera.hpp"
#include "Chicane/Runtime/Scene/Actor/Sky.hpp"

#include "Actor.hpp"
#include "Game.hpp"

static inline constexpr const std::uint32_t APPLE_DEPTH_COUNT  = 2;
static inline constexpr const std::uint32_t APPLE_COLUMN_COUNT = 5;
static inline constexpr const std::uint32_t APPLE_ROW_COUNT    = 2;
static inline constexpr const float         APPLE_STEP         = 20.0f;

Level::Level()
    : Chicane::Scene(),
    m_leftCamera(nullptr),
    m_centerCamera(nullptr),
    m_rightCamera(nullptr)
{}

void Level::onActivation()
{
    spawnSky();
    spawnLights();
    spawnCameras();
    spawnStructures();
    spawnApples();

    activateCenterCamera();
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
    createActor<Chicane::ASky>()->setSky(
        Chicane::Box::load<Chicane::Box::Sky>("Contents/Engine/Skies/Gray.bsky")
    );
}

void Level::spawnLights()
{
    createActor<Sun>();
}

void Level::spawnCameras()
{
    m_leftCamera = createActor<Chicane::ACamera>();
    m_leftCamera->setAbsoluteTranslation(-500.0f, -500.0f, 100.0f);
    m_leftCamera->setAbsoluteRotation(-20.0f, 0.0f, -45.0f);

    m_centerCamera = createActor<Chicane::ACamera>();
    m_centerCamera->setAbsoluteTranslation(0.0f, -500.0f, 100.0f);
    m_centerCamera->setAbsoluteRotation(-20.0f, 0.0f, 0.0f);

    m_rightCamera = createActor<Chicane::ACamera>();
    m_rightCamera->setAbsoluteTranslation(500.0f, -500.0f, 100.0f);
    m_rightCamera->setAbsoluteRotation(-20.0f, 0.0f, 45.0f);
}

void Level::spawnStructures()
{
    createActor<Strcuture>()->setAbsoluteScale(500.0f, 500.0f, 0.25f);
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

            position.x  = startPosition.x;
            position.z += APPLE_STEP;
        }

        position.y += APPLE_STEP;
        position.z  = startPosition.z;
    }
}