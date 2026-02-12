#include "Application.hpp"

#include <Chicane/Core/Window/Backend.hpp>
#include <Chicane/Core/Window/Type.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Application/CreateInfo.hpp>

#include "Actor/Character.hpp"

Application::Application()
{
    Chicane::ApplicationCreateInfo createInfo = {};

    // Window
    createInfo.window.title   = "Shooter Sample";
    createInfo.window.size    = Chicane::Vec<2, std::uint32_t>(1600, 900);
    createInfo.window.display = 0;
    createInfo.window.type    = Chicane::WindowType::Windowed;
    createInfo.window.backend = Chicane::WindowBackend::Vulkan;

    // Setup
    createInfo.onSetup = [this]()
    {
        initLevel();
        initCharacter();
        initView();
    };

    Chicane::Application::getInstance().run(createInfo);
}

void Application::initLevel()
{
    Chicane::Application::getInstance().setScene<Level>();
}

void Application::initCharacter()
{
    Chicane::Application& application = Chicane::Application::getInstance();

    m_controller = std::make_unique<Chicane::Controller>();
    application.setController(m_controller.get());

    Character* character = application.getScene<Level>()->createActor<Character>();
    character->setAbsoluteTranslation(Chicane::Vec3(0.0f, -150.0f, 0.0f));

    m_controller->attachTo(character);
}

void Application::initView()
{
    Chicane::Application::getInstance().setView<View>();
}