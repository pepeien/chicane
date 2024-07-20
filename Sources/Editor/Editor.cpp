#include "Editor/Editor.hpp"

#include <filesystem>

#include "Runtime/Grid.hpp"

#include "Editor/Camera.hpp"
#include "Editor/View.hpp"
#include "Editor/Layer.hpp"

namespace Chicane
{
    namespace Editor
    {
        void run()
        {
            std::unique_ptr<ViewportCamera> camera = std::make_unique<ViewportCamera>();
            State::setCamera(camera.get());

            std::unique_ptr<Level> level = std::make_unique<Level>();
            level->setSkybox(Box::read("Content/Editor/Textures/SKY_Gray.box"));
            State::setLevel(level.get());

            std::unique_ptr<View> view = std::make_unique<View>();
            Grid::addView(view.get());
            Grid::setActiveView(view->getId());

            WindowCreateInfo windowCreateInfo = {};
            windowCreateInfo.title         = "Chicane Editor";
            windowCreateInfo.resolution.x  = 1600;
            windowCreateInfo.resolution.y  = 900;
            windowCreateInfo.type          = WindowType::Windowed;
            windowCreateInfo.displayIndex  = 1;

            std::unique_ptr<Window> window = std::make_unique<Window>(windowCreateInfo);
            window->addLayer(
                new GridLayer(window.get()),
                LayerPushTecnique::BeforeLayer,
                "Level"
            );
            window->run();
        }
    }
}
