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

    Chicane::Application::CreateInfo createInfo {};
    createInfo.window   = windowCreateInfo;
    createInfo.renderer = Chicane::Renderer::Type::Vulkan;
    createInfo.onSetup   = [this]() {
        initCubeMap();
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

void Application::initCubeMap()
{
    Chicane::Loader::loadCubemap("Content/Sample/Cubemaps/Gray.bcmp");
}

void Application::initLevel()
{
    m_level = std::make_unique<Level>();
    m_level->activate();
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

void Application::initView()
{
    m_view = std::make_unique<View>();
    m_view->activate();
}