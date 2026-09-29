#pragma once

#include <memory>
#include <vector>

#include <Chicane/Core/FileSystem/Path.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Editor/Scene.hpp"
#include "Editor/Viewer/Scene.hpp"

namespace Editor
{
    class Application : public Chicane::Application
    {
    public:
        static Application& sInstance();

    public:
        Application(int inArgCount, char* inArgValues[]);

    public:
        std::shared_ptr<Scene> getHomeScene() const;
        std::shared_ptr<ViewerScene> getViewerScene() const;

        void activateHomeScene();
        void activateViewerScene();

        void possess(const std::shared_ptr<Chicane::Scene>& inScene);

    private:
        void initController();
        void initScene();
        void initView();
        void initLayers();

    private:
        static Application* s_instance;

        std::unique_ptr<Chicane::Controller> m_controller;
        std::shared_ptr<Scene>               m_homeScene;
        std::shared_ptr<ViewerScene>         m_viewerScene;
    };
}
