#include "Sample/Application.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Application/CreateInfo.hpp>

#include "Sample/Shooter/Scene.hpp"
#include "Sample/Shooter/UI/View/Home.hpp"

Application::Application()
{
    Chicane::ApplicationCreateInfo createInfo;

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
        initView();
    };

    Chicane::Application::getInstance().run(createInfo);
}

void Application::initController()
{
    m_controller = std::make_unique<Chicane::Controller>();

    Chicane::Application::getInstance().setController(m_controller.get());
}

void Application::initScene()
{
    Chicane::Application::getInstance().setScene<Scene>();
}

void Application::initView()
{
    Chicane::Application::getInstance().setView<HomeView>();
}