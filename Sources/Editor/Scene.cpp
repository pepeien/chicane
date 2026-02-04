#include "Scene.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Actor/Sky.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

#include "Actor/Character.hpp"
#include "Actor/Sun.hpp"

namespace Editor
{
    Scene::Scene()
        : Chicane::Scene()
    {}

    void Scene::onActivation()
    {
        spawnSky();
        spawnLights();
        spawnCharacter();
    }

    void Scene::spawnSky()
    {
        createActor<Chicane::ASky>()->setSky(
            Chicane::Box::load<Chicane::Box::Sky>("Contents/Engine/Skies/Color/Black.bsky")
        );
    }

    void Scene::spawnLights()
    {
        createActor<Sun>();
    }

    void Scene::spawnCharacter()
    {
        Character* character = createActor<Character>();
        character->setAbsoluteTranslation(10.0f, -10.0f, 10.0f);
        character->look(45.0f, -35.0f);

        Chicane::Application::getInstance().getController()->attachTo(character);
    }
}