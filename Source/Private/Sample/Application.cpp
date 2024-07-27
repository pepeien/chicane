#include "Application.hpp"

#include "Actor/Apple.hpp"
#include "Actor/Floor.hpp"

Application::Application()
{
    initCamera();
    initLevel();
    addFloor();
    addApples();
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
    Chicane::Allocator::load("Content/Sample/Texture/Skybox/Sunset.box");

    m_level = std::make_unique<Chicane::Level>();

    Chicane::State::setLevel(m_level.get());
}

void Application::addFloor()
{
    Floor* floor = new Floor();
    m_level->addActor(floor);
}

void Application::addApples()
{
    Apple* appleTopRight = new Apple();
    appleTopRight->setAbsoluteTranslation(Chicane::Vec<float>::Three(1000.0f, 1000.0f, 10.0f));
    m_level->addActor(appleTopRight);

    Apple* appleTopLeft = new Apple();
    appleTopLeft->setAbsoluteTranslation(Chicane::Vec<float>::Three(0.0f, 0.0f, 10.0f));
    m_level->addActor(appleTopLeft);

    Apple* appleMiddle = new Apple();
    appleMiddle->setAbsoluteTranslation(Chicane::Vec<float>::Three(1000.0f, -1000.0f, 10.0f));
    m_level->addActor(appleMiddle);

    Apple* appleBottomRight = new Apple();
    appleBottomRight->setAbsoluteTranslation(Chicane::Vec<float>::Three(-1000.0f, -1000.0f, 10.0f));
    m_level->addActor(appleBottomRight);

    Apple* appleBottomLeft = new Apple();
    appleBottomLeft->setAbsoluteTranslation(Chicane::Vec<float>::Three(-1000.0f, 1000.0f, 10.0f));
    m_level->addActor(appleBottomLeft);
}

void Application::addView()
{
    m_view = std::make_unique<View>();

    Chicane::Grid::addView(m_view.get());
    Chicane::Grid::setActiveView(m_view->getId());
}

void Application::initWindow()
{
    Chicane::WindowCreateInfo windowCreateInfo {};
    windowCreateInfo.title         = "Chicane Sample";
    windowCreateInfo.resolution.x  = 1600;
    windowCreateInfo.resolution.y  = 900;
    windowCreateInfo.type          = Chicane::WindowType::Windowed;
    windowCreateInfo.displayIndex  = 0;

    m_window = std::make_unique<Chicane::Window>(windowCreateInfo);
}