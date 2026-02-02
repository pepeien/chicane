#include "Scene.hpp"

#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Actor/Sky.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

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
}