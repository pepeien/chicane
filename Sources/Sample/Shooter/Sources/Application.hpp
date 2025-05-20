#pragma once

#include "Essential.hpp"

#include "View.hpp"
#include "Level.hpp"

class Application
{
public:
    Application();
    ~Application();

private:
    void initLevel();
    void initCharacter();
    void initView();

private:
    std::unique_ptr<Chicane::Controller> m_controller;
    std::unique_ptr<Level>                         m_level;
    std::unique_ptr<View>                          m_view;
};