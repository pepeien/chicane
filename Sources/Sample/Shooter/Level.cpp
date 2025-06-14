#include "Level.hpp"

#include "Actor.hpp"
#include "Game.hpp"

static inline constexpr const std::uint32_t APPLE_DEPTH_COUNT  = 2;
static inline constexpr const std::uint32_t APPLE_COLUMN_COUNT = 5;
static inline constexpr const std::uint32_t APPLE_ROW_COUNT    = 2;
static inline constexpr const float         APPLE_STEP         = 20.0f;

Level::Level()
    : Chicane::Level()
{}

void Level::onActivation()
{
    spawnSky();
    spawnLights();
    spawnStructures();
    spawnApples();
}

void Level::spawnSky()
{
    createActor<Chicane::ASky>()->setSky(
        Chicane::Box::load<Chicane::Box::Sky::Instance>("Contents/Engine/Skies/Gray.bsky")
    );
}

void Level::spawnLights()
{
    createActor<Sun>();
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