#include "Editor/Editor.hpp"

#include "Runtime/Grid.hpp"

#include <filesystem>

namespace Chicane
{
    namespace Editor
    {
        void run()
        {
            std::unique_ptr<Level> level = std::make_unique<Level>();
            std::unique_ptr<Controller> controller = std::make_unique<Controller>();

            State::setController(controller.get());

            Box::Instance skybox = Box::read("Content/Textures/SKY_Grid");
            level->setSkybox(skybox);

            WindowCreateInfo windowCreateInfo = {};
            windowCreateInfo.title         = "Chicane Editor";
            windowCreateInfo.resolution.x  = 1600;
            windowCreateInfo.resolution.y  = 900;
            windowCreateInfo.type          = WindowType::Windowed;
            windowCreateInfo.displayIndex  = 0;

            Grid::View* homeView = new Grid::View("home", "Content/UI/Home.xml");

            Grid::addView(homeView);
            Grid::setActiveView("home");

            std::unique_ptr<Window> window = std::make_unique<Window>(
                windowCreateInfo,
                controller.get(),
                level.get()
            );
            window->run();
        }
    }
}
