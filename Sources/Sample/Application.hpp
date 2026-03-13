#pragma once

#include <Chicane/Runtime/Controller.hpp>

class Application
{
public:
    Application();

private:
    void initController();

private:
    std::unique_ptr<Chicane::Controller> m_controller;
};