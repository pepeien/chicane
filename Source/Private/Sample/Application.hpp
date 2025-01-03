#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Renderer.hpp"
#include "Chicane/Base.hpp"

#include "View.hpp"
#include "Level.hpp"

class Application
{
public:
    Application();

public:
    void run();

private:
    void initChacater();
    void initLevel();
    void initView();
    void initWindow();

private:
    std::unique_ptr<Chicane::Controller> m_controller;
    std::unique_ptr<Level> m_level;
    std::unique_ptr<View> m_view;
    std::unique_ptr<Chicane::Window::Instance> m_window;
};