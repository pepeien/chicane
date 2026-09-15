#pragma once

#include <memory>

#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Editor/Scene.hpp"
#include "Editor/Viewer/Scene.hpp"

namespace Editor
{
    class Application
    {
    public:
        static Application& getInstance();

    public:
        Application();

        std::shared_ptr<Scene> getHomeScene() const;
        std::shared_ptr<ViewerScene> getViewerScene() const;

        void activateHomeScene();
        void activateViewerScene();

    private:
        void initController();
        void initScene();
        void initView();
        void initLayers();
        void possess(const std::shared_ptr<Chicane::Scene>& inScene);

    private:
        static Application* s_instance;

        std::unique_ptr<Chicane::Controller> m_controller;
        std::shared_ptr<Scene>               m_homeScene;
        std::shared_ptr<ViewerScene>         m_viewerScene;
    };
}
