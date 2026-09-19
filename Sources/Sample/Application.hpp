#pragma once

#include <Chicane/Runtime/Controller.hpp>

class Application
{
public:
    Application();

private:
    void initController();
    void initScene();

private:
    std::unique_ptr<Chicane::Controller> m_controller;
};