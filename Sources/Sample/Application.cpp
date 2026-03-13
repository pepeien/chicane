#include "Sample/Application.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Application/CreateInfo.hpp>

Application::Application()
{
    Chicane::ApplicationCreateInfo createInfo;

    // Window
    createInfo.window.title   = "Chicane Sample";
    createInfo.window.size    = Chicane::Vec<2, std::uint32_t>(1600, 900);
    createInfo.window.display = 0;
    createInfo.window.type    = Chicane::WindowType::Windowed;
    createInfo.window.backend = Chicane::WindowBackend::OpenGL;

    // Setup
    createInfo.onSetup = [this]() { initController(); };

    Chicane::Application::getInstance().run(createInfo);
}

void Application::initController()
{
    m_controller = std::make_unique<Chicane::Controller>();

    Chicane::Application::getInstance().setController(m_controller.get());
}