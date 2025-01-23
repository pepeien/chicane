#include "Level.hpp"

#include "Game.hpp"
#include "Actor/Apple.hpp"
#include "Actor/Structure.hpp"
#include "Actor/Sun.hpp"

static constexpr std::uint32_t APPLE_DEPTH_COUNT  = 1;
static constexpr std::uint32_t APPLE_COLUMN_COUNT = 10;
static constexpr std::uint32_t APPLE_ROW_COUNT    = 1;
static constexpr float         APPLE_STEP         = 20.0f;

Level::Level()
    : Chicane::Level()
{}

void Level::onActivation()
{
    spawnLights();
    spawnStructures();
    spawnApples();
}

void Level::spawnLights()
{
    addActor(new Sun());
}

void Level::spawnStructures()
{
    Strcuture* floor = new Strcuture();
    floor->setScale(
        Chicane::Vec<3, float>(
            200.0f,
            200.0f,
            0.25f
        )
    );
    addActor(floor);
}

void Level::spawnApples()
{
    const Chicane::Vec<3, float> startPosition(
        -(APPLE_ROW_COUNT * APPLE_STEP),
        -((APPLE_DEPTH_COUNT * APPLE_STEP) * 0.25f),
        100.0f
    );
    Chicane::Vec<3, float> position = startPosition;

    Game::setMaxScore(APPLE_ROW_COUNT * APPLE_COLUMN_COUNT * APPLE_DEPTH_COUNT);

    for (std::uint32_t depth = 0; depth < APPLE_DEPTH_COUNT; depth++)
    {
        for (std::uint32_t row = 0; row < APPLE_ROW_COUNT; row++)
        {
            for (std::uint32_t column = 0; column < APPLE_COLUMN_COUNT; column++)
            {
                Apple* apple = new Apple(
                    column % 2 == 0 ? "Content/Sample/Meshes/Apple.bmsh" : "Content/Sample/Meshes/Apple_Sun.bmsh",
                    position
                );
                addActor(apple);

                position.x += APPLE_STEP;
            }

            position.x  = startPosition.x;
            position.z += APPLE_STEP;
        }

        position.y += APPLE_STEP;
        position.z  = startPosition.z;
    }
}