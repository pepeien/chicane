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

        Chicane::Actor* actor = createActor<Chicane::Actor>();

        Chicane::CMesh* mesh = createComponent<Chicane::CMesh>();
        mesh->setMesh("Contents/Engine/Meshes/Shape/Cube.bmsh");
        mesh->attachTo(actor);
        mesh->activate();
    }

    void Scene::spawnSky()
    {
        createActor<Chicane::ASky>()->setSky(Chicane::Box::load<Chicane::Box::Sky>("Contents/Engine/Skies/Debug.bsky"));
    }

    void Scene::spawnLights()
    {
        createActor<Sun>();
    }
}