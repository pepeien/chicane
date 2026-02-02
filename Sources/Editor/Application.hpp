#pragma once

#include <memory>

#include <Chicane/Runtime/Controller.hpp>

namespace Editor
{
    class Application
    {
    public:
        Application();

    private:
        void initScene();
        void initCharacter();
        void initView();
        void initLayers();

    private:
        std::unique_ptr<Chicane::Controller> m_controller;
    };
}