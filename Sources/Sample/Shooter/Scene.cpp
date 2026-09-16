#include "Sample/Shooter/Scene.hpp"

#include <Chicane/Kerb/Engine.hpp>
#include <Chicane/Kerb/Gravity.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene/Actor/Camera.hpp>

#include "Actor/Apple.hpp"

#include "Game.hpp"

static constexpr inline const std::uint32_t APPLE_DEPTH_COUNT   = 2;
static constexpr inline const std::uint32_t APPLE_COLUMN_COUNT  = 5;
static constexpr inline const std::uint32_t APPLE_ROW_COUNT     = 2;
static constexpr inline const float         APPLE_STEP          = 20.0f;
static constexpr inline float               WORLD_GRAVITY_SCALE = 10.0f;

Scene::Scene()
    : Chicane::Scene()
{}

void Scene::onLoad()
{
    Chicane::Kerb::Engine::setGravity(
        Chicane::Kerb::Gravity::down(Chicane::Kerb::Gravity::Earth * WORLD_GRAVITY_SCALE)
    );

    open(DEFAULT_TRACK);

    spawnApples();
}

void Scene::activateLeftCamera()
{
    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(LEFT_CAMERA_ID))
    {
        camera->activate();
    }
    else
    {
        return;
    }

    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(CENTER_CAMERA_ID))
    {
        camera->deactivate();
    }

    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(RIGHT_CAMERA_ID))
    {
        camera->deactivate();
    }
}

void Scene::activateCenterCamera()
{
    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(CENTER_CAMERA_ID))
    {
        camera->activate();
    }
    else
    {
        return;
    }

    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(LEFT_CAMERA_ID))
    {
        camera->deactivate();
    }

    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(RIGHT_CAMERA_ID))
    {
        camera->deactivate();
    }
}

void Scene::activateRightCamera()
{
    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(RIGHT_CAMERA_ID))
    {
        camera->activate();
    }
    else
    {
        return;
    }

    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(LEFT_CAMERA_ID))
    {
        camera->deactivate();
    }

    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(CENTER_CAMERA_ID))
    {
        camera->deactivate();
    }
}

void Scene::disableCameras()
{
    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(LEFT_CAMERA_ID))
    {
        camera->deactivate();
    }
    else
    {
        return;
    }

    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(CENTER_CAMERA_ID))
    {
        camera->deactivate();
    }

    if (Chicane::ACamera* camera = getActor<Chicane::ACamera>(RIGHT_CAMERA_ID))
    {
        camera->deactivate();
    }
}

void Scene::spawnApples()
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
