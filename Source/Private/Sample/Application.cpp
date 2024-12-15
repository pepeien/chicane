#include "Application.hpp"

#include "Actor/Character.hpp"

Application::Application()
{
    initLevel();
    initChacater();
    initView();
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
    m_level = std::make_unique<Level>();

    Chicane::setActiveLevel(m_level.get());
}

void Application::initView()
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