#include "Editor/Editor.hpp"

#include "Runtime/Grid.hpp"
#include "Editor/View.hpp"
#include "Editor/Layer.hpp"

#include <filesystem>

namespace Chicane
{
    namespace Editor
    {
        void run()
        {
            std::unique_ptr<Level> level           = std::make_unique<Level>();
            std::unique_ptr<Controller> controller = std::make_unique<Controller>();
            std::unique_ptr<View> view             = std::make_unique<View>();

            Actor* actor = new Actor();
            actor->setModel(Box::read("Content/Meshes/Cube.box"));

            level->setSkybox(Box::read("Content/Textures/SKY_Gray.box"));
            level->addActor(actor);

            WindowCreateInfo windowCreateInfo = {};
            windowCreateInfo.title         = "Chicane Editor";
            windowCreateInfo.resolution.x  = 1600;
            windowCreateInfo.resolution.y  = 900;
            windowCreateInfo.type          = WindowType::Windowed;
            windowCreateInfo.displayIndex  = 0;

            Grid::addView(view.get());
            Grid::setActiveView(view->getId());

            std::unique_ptr<Window> window = std::make_unique<Window>(
                windowCreateInfo,
                controller.get(),
                level.get()
            );

            window->addLayer(
                new EditorLayer(window.get()),
                LayerPushTecnique::BeforeLayer,
                "Level"
            );

            window->run();
        }
    }
}
