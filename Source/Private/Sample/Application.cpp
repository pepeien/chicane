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

    Chicane::setCamera(m_camera.get());
}

void Application::initLevel()
{
    Chicane::Allocator::load("Content/Sample/Texture/Skybox/Sunset.box");

    m_level = std::make_unique<Chicane::Level>();

    Chicane::setLevel(m_level.get());
}

void Application::addFloor()
{
    Floor* floor = new Floor();
    m_level->addActor(floor);
}

void Application::addApples()
{
    Chicane::Vec<float>::Three topLeftPosition(     1000.0f,  1000.0f, 0.0f);
    Chicane::Vec<float>::Three topRightPosition(    0.0f,     0.0f,    0.0f);
    Chicane::Vec<float>::Three middlePosition(      1000.0f, -1000.0f, 0.0f);
    Chicane::Vec<float>::Three bottomLeftPosition( -1000.0f, -1000.0f, 0.0f);
    Chicane::Vec<float>::Three bottomRightPosition(-1000.0f,  1000.0f, 0.0f);

    for (std::uint32_t i = 0; i < 10; i++)
    {
        float height = 10.0f + (i * 200);

        topLeftPosition.z     = height;
        topRightPosition.z    = height;
        middlePosition.z      = height;
        bottomLeftPosition.z  = height;
        bottomRightPosition.z = height;

        Apple* appleTopRight = new Apple();
        appleTopRight->setAbsoluteTranslation(topLeftPosition);
        m_level->addActor(appleTopRight);

        Apple* appleTopLeft = new Apple();
        appleTopLeft->setAbsoluteTranslation(topRightPosition);
        m_level->addActor(appleTopLeft);

        Apple* appleMiddle = new Apple();
        appleMiddle->setAbsoluteTranslation(middlePosition);
        m_level->addActor(appleMiddle);

        Apple* appleBottomRight = new Apple();
        appleBottomRight->setAbsoluteTranslation(bottomLeftPosition);
        m_level->addActor(appleBottomRight);

        Apple* appleBottomLeft = new Apple();
        appleBottomLeft->setAbsoluteTranslation(bottomRightPosition);
        m_level->addActor(appleBottomLeft);
    }
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