#include "Level.hpp"

#include "Game.hpp"
#include "Actor/Apple.hpp"
#include "Actor/Structure.hpp"

constexpr std::uint32_t APPLE_DEPTH_COUNT  = 1;
constexpr std::uint32_t APPLE_COLUMN_COUNT = 10;
constexpr std::uint32_t APPLE_ROW_COUNT    = 1;
constexpr float         APPLE_STEP         = 20.0f;

Level::Level()
    : Chicane::Level()
{}

void Level::onActivation()
{
    Chicane::Loader::loadCubemap("Content/Sample/Texture/Skybox/Sunset.box");

    spawnStructures();
    spawnApples();
}

void Level::spawnStructures()
{
    const Chicane::Vec<3, float> floorScale(
        200.0f,
        200.0f,
        0.25f
    );

    Strcuture* floor = new Strcuture();
    floor->setAbsoluteScale(floorScale);
    addActor(floor);

    const Chicane::Vec<3, float> scale(
        200.0f,
        50.0f,
        0.25f
    );

    Strcuture* frontWall = new Strcuture();
    frontWall->setAbsoluteScale(scale);
    frontWall->setAbsoluteRotation(
        Chicane::Vec<3, float>(
            90.0f,
            0.0f,
            0.0f
        )
    );
    frontWall->setAbsoluteTranslation(
        Chicane::Vec<3, float>(
            0.0f,
            floorScale.y,
            scale.y * 0.5f
        )
    );
    addActor(frontWall);

    Strcuture* backWall = new Strcuture();
    backWall->setAbsoluteScale(scale);
    backWall->setAbsoluteRotation(
        Chicane::Vec<3, float>(
            90.0f,
            0.0f,
            0.0f
        )
    );
    backWall->setAbsoluteTranslation(
        Chicane::Vec<3, float>(
            0.0f,
            -floorScale.y,
            scale.y * 0.5f
        )
    );
    addActor(backWall);

    Strcuture* leftWall = new Strcuture();
    leftWall->setAbsoluteScale(scale);
    leftWall->setAbsoluteRotation(
        Chicane::Vec<3, float>(
            90.0f,
            90.0f,
            0.0f
        )
    );
    leftWall->setAbsoluteTranslation(
        Chicane::Vec<3, float>(
            -floorScale.x,
            0.0f,
            scale.y * 0.5f
        )
    );
    addActor(leftWall);

    Strcuture* rightWall = new Strcuture();
    rightWall->setAbsoluteScale(scale);
    rightWall->setAbsoluteRotation(
        Chicane::Vec<3, float>(
            90.0f,
            90.0f,
            0.0f
        )
    );
    rightWall->setAbsoluteTranslation(
        Chicane::Vec<3, float>(
            floorScale.x,
            0.0f,
            scale.y * 0.5f
        )
    );
    addActor(rightWall);
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
                    row % 2 == 0 ? "Content/Sample/Meshes/Apple.box" : "Content/Sample/Meshes/Apple_Sun.box",
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