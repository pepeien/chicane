#include "Application.hpp"

#include "Actor/Apple.hpp"
#include "Actor/Character.hpp"
#include "Actor/Structure.hpp"
#include "Game.hpp"

constexpr std::uint32_t APPLE_DEPTH_COUNT  = 1;
constexpr std::uint32_t APPLE_COLUMN_COUNT = 10;
constexpr std::uint32_t APPLE_ROW_COUNT    = 5;
constexpr float         APPLE_STEP         = 20.0f;

Application::Application()
{
    initLevel();
    initChacater();
    addStructures();
    addApples();
    addView();
    initWindow();
}

void Application::run()
{
    m_window->run();
}

void Application::initChacater()
{
    m_controller = std::make_unique<Chicane::Controller>();
    Chicane::setActiveController(m_controller.get());

    Character* character = new Character();
    character->setAbsoluteTranslation(Chicane::Vec<3, float>(0.0f, -150.0f, 20.0f));

    m_controller->attachTo(character);

    m_level->addActor(character);
}

void Application::initLevel()
{
    Chicane::Loader::loadCubemap("Content/Sample/Texture/Skybox/Sunset.box");

    m_level = std::make_unique<Chicane::Level>();

    Chicane::setActiveLevel(m_level.get());
}

void Application::addStructures()
{
    const Chicane::Vec<3, float> floorScale(
        200.0f,
        200.0f,
        0.25f
    );

    Strcuture* floor = new Strcuture();
    floor->setAbsoluteScale(floorScale);
    m_level->addActor(floor);

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
    m_level->addActor(frontWall);

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
    m_level->addActor(backWall);

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
    m_level->addActor(leftWall);

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
    m_level->addActor(rightWall);
}

void Application::addApples()
{
    const Chicane::Vec<3, float> startPosition(
        -(APPLE_ROW_COUNT * APPLE_STEP),
        -((APPLE_DEPTH_COUNT * APPLE_STEP) * 0.25f),
        10.0f
    );
    Chicane::Vec<3, float> position = startPosition;

    Game::setMaxScore(APPLE_ROW_COUNT * APPLE_COLUMN_COUNT * APPLE_DEPTH_COUNT);

    for (std::uint32_t depth = 0; depth < APPLE_DEPTH_COUNT; depth++)
    {
        for (std::uint32_t row = 0; row < APPLE_ROW_COUNT; row++)
        {
            for (std::uint32_t column = 0; column < APPLE_COLUMN_COUNT; column++)
            {
                Apple* apple = new Apple();
                apple->setAbsoluteTranslation(position);
                m_level->addActor(apple);

                position.x += APPLE_STEP;
            }

            position.x  = startPosition.x;
            position.z += APPLE_STEP;
        }

        position.y += APPLE_STEP;
        position.z  = startPosition.z;
    }
}

void Application::addView()
{
    m_view = std::make_unique<View>();

    Chicane::Grid::addView(m_view.get());
    Chicane::Grid::setActiveView(m_view->getId());
}

void Application::initWindow()
{
    Chicane::WindowCreateInfo windowCreateInfo {};
    windowCreateInfo.title         = "Chicane Sample";
    windowCreateInfo.resolution.x  = 1600;
    windowCreateInfo.resolution.y  = 900;
    windowCreateInfo.type          = Chicane::WindowType::Windowed;
    windowCreateInfo.displayIndex  = 0;

    m_window = std::make_unique<Chicane::Window>(windowCreateInfo);
}