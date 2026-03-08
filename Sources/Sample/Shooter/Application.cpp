#include "Sample/Shooter/Application.hpp"

#include <Chicane/Core/Window/Backend.hpp>
#include <Chicane/Core/Window/Type.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Application/CreateInfo.hpp>

Application::Application()
{
    Chicane::ApplicationCreateInfo createInfo;

    // Window
    createInfo.window.title   = "Shooter Sample";
    createInfo.window.size    = Chicane::Vec<2, std::uint32_t>(1920, 1080);
    createInfo.window.display = 0;
    createInfo.window.type    = Chicane::WindowType::Fullscreen;
    createInfo.window.backend = Chicane::WindowBackend::OpenGL;

    // Setup
    createInfo.onSetup = [this]()
    {
        initController();
        initLevel();
        initView();
    };

    Chicane::Application::getInstance().run(createInfo);
}

void Application::initLevel()
{
    Chicane::Application::getInstance().setScene<Level>();
}

void Application::initController()
{
    m_controller = std::make_unique<Chicane::Controller>();

    Chicane::Application::getInstance().setController(m_controller.get());
}

void Application::initView()
{
    Chicane::Application::getInstance().setView<View>();
}