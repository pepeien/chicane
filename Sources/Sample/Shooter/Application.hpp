#pragma once

#include <Chicane/Runtime/Controller.hpp>

#include "Level.hpp"
#include "View.hpp"

class Application
{
public:
    Application();

private:
    void initController();
    void initLevel();
    void initView();

private:
    std::unique_ptr<Chicane::Controller> m_controller;
};