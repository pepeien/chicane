#include "Application.hpp"

#include "Actor/Apple.hpp"
#include "Actor/Character.hpp"

constexpr std::uint32_t APPLE_COLUMN_COUNT = 5;
constexpr std::uint32_t APPLE_ROW_COUNT    = 5;

Application::Application()
{
    initLevel();
    initChacater();
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
    character->setAbsoluteTranslation(Chicane::Vec<3, float>(0.0f, -110.0f, 40.0f));

    m_controller->attachTo(character);

    m_level->addActor(character);
}

void Application::initLevel()
{
    Chicane::Loader::loadCubemap("Content/Sample/Texture/Skybox/Sunset.box");

    m_level = std::make_unique<Chicane::Level>();

    Chicane::setActiveLevel(m_level.get());
}

void Application::addApples()
{
    float step = 20.0f;

    Chicane::Vec<3, float> position(0.0f);
    position.x -= (step / 2) * APPLE_COLUMN_COUNT;

    for (std::uint32_t column = 0; column < APPLE_COLUMN_COUNT; column++)
    {
        position.x += step;
        position.y += step;
        position.z = 0.0f;

        for (std::uint32_t row = 0; row < APPLE_ROW_COUNT; row++)
        {
            Apple* apple = new Apple();
            apple->setAbsoluteTranslation(position);

            m_level->addActor(apple);

            position.z += step;
        }
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