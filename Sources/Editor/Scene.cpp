#include "Editor/Scene.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Actor/Sky.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

#include "Editor/Actor/Character.hpp"
#include "Editor/Actor/Item.hpp"
#include "Editor/Actor/Sun.hpp"

namespace Editor
{
    Scene::Scene()
        : Chicane::Scene()
    {}

    void Scene::onLoad()
    {
        spawnSky();
        spawnLights();
        spawnCharacter();
        spawnDefaultItem();
    }

    void Scene::spawnSky()
    {
        createActor<Chicane::ASky>()->setSky(Chicane::Box::load<Chicane::Box::Sky>("Assets/Editor/Skies/Default.bsky"));
    }

    void Scene::spawnLights()
    {
        createActor<Sun>();
    }

    void Scene::spawnCharacter()
    {
        Chicane::Application::getInstance().getController()->attachTo(createActor<Character>());
    }

    void Scene::spawnDefaultItem()
    {
        createActor<Item>("Assets/Engine/Meshes/Shape/Cube.bmsh");
    }
}