#include "Application.hpp"

#include "Actor/Character.hpp"

Application::Application()
{
    Chicane::Application::CreateInfo createInfo = {};

    // Window
    createInfo.window.title   = "Chicane Sample";
    createInfo.window.size    = Chicane::Vec<2, int>(1600, 900);
    createInfo.window.type    = Chicane::Window::Type::Windowed;
    createInfo.window.display = 0;

    // Renderer
    createInfo.renderer.type = Chicane::Renderer::Type::Vulkan;

    // Setup
    createInfo.onSetup = [this]() {
        initLevel();
        initChacater();
        initView();
    };

    Chicane::Application::run(createInfo);
}

Application::~Application()
{
    Chicane::Application::stop();
}

void Application::initLevel()
{
    m_level = std::make_unique<Level>();
    m_level->activate();
}

void Application::initChacater()
{
    m_controller = std::make_unique<Chicane::Controller::Instance>();
    Chicane::Application::setController(m_controller.get());

    Character* character = m_level->createActor<Character>();
    character->setAbsoluteTranslation(Chicane::Vec<3, float>(0.0f, -150.0f, 20.0f));

    m_controller->attachTo(character);
}

void Application::initView()
{
    m_view = std::make_unique<View>();
    m_view->activate();
}