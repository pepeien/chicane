#include "Application.hpp"

#include "Chicane/Core/Window/Type.hpp"
#include "Chicane/Core/Window/Renderer.hpp"
#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Application/CreateInfo.hpp"

#include "Actor.hpp"

Application::Application()
{
    Chicane::ApplicationCreateInfo createInfo = {};

    // Window
    createInfo.window.title    = "Shooter Sample";
    createInfo.window.size     = Chicane::Vec<2, int>(1600, 900);
    createInfo.window.display  = 0;
    createInfo.window.type     = Chicane::WindowType::Windowed;
    createInfo.window.renderer = Chicane::WindowRenderer::Vulkan;

    // Setup
    createInfo.onSetup = [this]() {
        initLevel();
        initCharacter();
        initView();
    };

    Chicane::Application::run(createInfo);
}

void Application::initLevel()
{
    m_level = std::make_unique<Level>();

    Chicane::Application::setScene(m_level.get());
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