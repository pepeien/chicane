#include "Application.hpp"

#include "Actor/Character.hpp"

Application::Application()
{
    Chicane::Window::CreateInfo windowCreateInfo {};
    windowCreateInfo.title        = "Chicane Sample";
    windowCreateInfo.resolution.x = 1600;
    windowCreateInfo.resolution.y = 900;
    windowCreateInfo.type         = Chicane::Window::Type::Windowed;
    windowCreateInfo.display      = 0;

    Chicane::Application::CreateInfo applicationCreateInfo {};
    applicationCreateInfo.windowCreateInfo = windowCreateInfo;
    applicationCreateInfo.renderer         = Chicane::Renderer::Type::Vulkan;

    Chicane::Application::start(applicationCreateInfo);
}

void Application::run()
{
    initLevel();
    initChacater();
    initView();
}

void Application::initChacater()
{
    m_controller = std::make_unique<Chicane::Controller>();
    Chicane::Application::setController(m_controller.get());

    Character* character = new Character();
    character->setAbsoluteTranslation(Chicane::Vec<3, float>(0.0f, -150.0f, 20.0f));

    m_controller->attachTo(character);

    m_level->addActor(character);
}

void Application::initLevel()
{
    m_level = std::make_unique<Level>();

    Chicane::Application::setLevel(m_level.get());
}

void Application::initView()
{
    m_view = std::make_unique<View>();

    Chicane::Grid::addView(m_view.get());
    Chicane::Grid::setActiveView(m_view->getId());
}