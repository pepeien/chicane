#include "Main.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Renderer.hpp"
#include "Chicane/Base.hpp"

#include "Apple.hpp"
#include "Camera.hpp"

int main(int argc, char *argv[])
{
    try
    {
        std::unique_ptr<Camera> camera = std::make_unique<Camera>();
        Chicane::State::setCamera(camera.get());

        Chicane::Actor* floor = new Chicane::Actor();
        floor->setMesh(Chicane::Box::read("Content/Sample/Meshes/Cube.box"));
        floor->setAbsoluteScale(glm::vec3(2500.0f, 2500.0f, 10.0f));

        Apple* apple = new Apple();
        apple->setAbsoluteTranslation(glm::vec3(0.0f, 0.0f, 10.0f));

        std::unique_ptr<Chicane::Level> level = std::make_unique<Chicane::Level>();
        level->addActor(floor);
        level->addActor(apple);
        level->setSkybox(Chicane::Box::read("Content/Sample/Texture/SKY_Sunset.box"));
        Chicane::State::setLevel(level.get());

        Chicane::WindowCreateInfo windowCreateInfo = {};
        windowCreateInfo.title         = "Chicane Sample";
        windowCreateInfo.resolution.x  = 1600;
        windowCreateInfo.resolution.y  = 900;
        windowCreateInfo.type          = Chicane::WindowType::Windowed;
        windowCreateInfo.displayIndex  = 0;

        std::unique_ptr<Chicane::Window> window = std::make_unique<Chicane::Window>(windowCreateInfo);
        window->run();
    }
    catch (const std::exception& e)
    {
        LOG_CRITICAL(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}