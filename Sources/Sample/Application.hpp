#pragma once

#include <memory>

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Controller.hpp>

class Application : public Chicane::Application
{
public:
    Application(int inArgCount, char* inArgValues[]);

private:
    void initController();
    void initScene();

private:
    std::unique_ptr<Chicane::Controller> m_controller;
};
