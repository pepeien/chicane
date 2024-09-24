#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Renderer.hpp"
#include "Chicane/Base.hpp"

#include "View.hpp"

class Application
{
public:
    Application();

public:
    void run();

private:
    void initChacater();
    void initLevel();
    void addStructures();
    void addApples();
    void addView();
    void initWindow();

private:
    std::unique_ptr<Chicane::Controller> m_controller;
    std::unique_ptr<Chicane::Level> m_level;
    std::unique_ptr<View> m_view;
    std::unique_ptr<Chicane::Window> m_window;
};