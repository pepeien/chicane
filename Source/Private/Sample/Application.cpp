#include "Application.hpp"

Application::Application()
{
    initCamera();
    initLevel();
    addActors();
    addView();
    initWindow();
}

void Application::run()
{
    m_window->run();
}

void Application::initCamera()
{
    m_camera = std::make_unique<Camera>();

    Chicane::State::setCamera(m_camera.get());
}

void Application::initLevel()
{
    m_level = std::make_unique<Chicane::Level>();;
    m_level->setSkybox(
        Chicane::Box::read(
            "Content/Sample/Texture/SKY_Sunset.box"
        )
    );

    Chicane::State::setLevel(m_level.get());
}

void Application::addActors()
{
    Chicane::Actor* floor = new Chicane::Actor();
    floor->setMesh(Chicane::Box::read("Content/Sample/Meshes/Cube.box"));
    floor->setAbsoluteScale(Chicane::Vec<float>::Three(2500.0f, 2500.0f, 10.0f));

    Apple* apple = new Apple();
    apple->setAbsoluteTranslation(Chicane::Vec<float>::Three(0.0f, 0.0f, 10.0f));

    m_level->addActor(floor);
    m_level->addActor(apple);
}

void Application::addView()
{
    m_view = std::make_unique<View>();

    Chicane::Grid::addView(m_view.get());
    Chicane::Grid::setActiveView(m_view->getId());
}

void Application::initWindow()
{
    Chicane::WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.title         = "Chicane Sample";
    windowCreateInfo.resolution.x  = 1600;
    windowCreateInfo.resolution.y  = 900;
    windowCreateInfo.type          = Chicane::WindowType::Windowed;
    windowCreateInfo.displayIndex  = 0;

    m_window = std::make_unique<Chicane::Window>(windowCreateInfo);
}