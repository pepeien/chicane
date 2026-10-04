#include "Sample/Application.hpp"

#include <Chicane/Core/Window/Backend.hpp>
#include <Chicane/Core/Window/Type.hpp>
#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Instance/CreateInfo.hpp>

#include "Sample/Shooter/Game.hpp"

Application::Application(int inArgCount, char* inArgValues[])
    : Chicane::Application(inArgCount, inArgValues)
{
    Chicane::InstanceCreateInfo createInfo;

    // Window
    createInfo.window.title   = "Chicane Sample";
    createInfo.window.display = 0;
    createInfo.window.type    = Chicane::WindowType::Windowed;
    createInfo.window.backend = Chicane::WindowBackend::Vulkan;

    // Setup
    createInfo.onSetup = [this]()
    {
        initController();
        initScene();
    };

    Chicane::Instance::sInstance().run(createInfo);
}

void Application::initController()
{
    m_controller = std::make_unique<Chicane::Controller>();

    Chicane::Instance::sInstance().setController(m_controller.get());
}

void Application::initScene()
{
    Game::boot();
}
