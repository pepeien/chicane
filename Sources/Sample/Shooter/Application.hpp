#pragma once

#include <Chicane/Runtime/Controller.hpp>

#include "Level.hpp"
#include "View.hpp"

class Application
{
public:
    Application();

private:
    void initLevel();
    void initCharacter();
    void initView();

private:
    std::unique_ptr<Chicane::Controller> m_controller;
    std::unique_ptr<Level>               m_level;
    std::unique_ptr<View>                m_view;
};