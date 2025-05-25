#include "Application.hpp"

#include "Actor.hpp"

Application::Application()
{
    Chicane::Application::CreateInfo createInfo = {};

    // Window
    createInfo.window.title   = "Shooter Sample";
    createInfo.window.size    = Chicane::Vec<2, int>(1600, 900);
    createInfo.window.display = 0;
    createInfo.window.type    = Chicane::Window::Type::Windowed;

    // Renderer
    createInfo.renderer.resolution = Chicane::Vec2(1660.0f, 900.0f);
    createInfo.renderer.type       = Chicane::Renderer::Type::Vulkan;

    // Setup
    createInfo.onSetup = [this]() {
        initLevel();
        initCharacter();
        //initView();
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

    Chicane::Application::setLevel(m_level.get());
}

void Application::initCharacter()
{
    m_controller = std::make_unique<Chicane::Controller>();
    Chicane::Application::setController(m_controller.get());

    Character* character = m_level->createActor<Character>();
    character->setAbsoluteTranslation(Chicane::Vec3(0.0f, -150.0f, 20.0f));

    m_controller->attachTo(character);
}

void Application::initView()
{
    m_view = std::make_unique<View>();

    Chicane::Application::setView(m_view.get());
}