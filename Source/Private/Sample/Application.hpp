#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Renderer.hpp"
#include "Chicane/Base.hpp"

#include "Camera.hpp"
#include "View.hpp"

class Application
{
public:
    Application();

public:
    void run();

private:
    void initCamera();
    void initLevel();
    void addFloor();
    void addApples();
    void addView();
    void initWindow();

private:
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Chicane::Level> m_level;
    std::unique_ptr<View> m_view;
    std::unique_ptr<Chicane::Window> m_window;
};