#include "Editor/Scene.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene/Actor/Sky.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

#include "Editor/Actor/Character.hpp"
#include "Editor/Actor/Item.hpp"
#include "Editor/Actor/Studio.hpp"

namespace Editor
{
    Scene::Scene()
        : Chicane::Scene(),
          m_gizmo(nullptr)
    {}

    void Scene::onLoad()
    {
        spawnSky();
        spawnLights();
        spawnCharacter();
        spawnDefaultItem();
        spawnGizmo();
    }

    void Scene::setSelection(Chicane::Object* inItem)
    {
        for (Chicane::CMesh* mesh : getComponents<Chicane::CMesh>())
        {
            mesh->setIsOutlined(inItem != nullptr && mesh->getParent() == inItem);
        }

        if (!m_gizmo)
        {
            return;
        }

        m_gizmo->setTarget(inItem);
    }

    void Scene::spawnSky()
    {
        Chicane::ASky* sky = createActor<Chicane::ASky>();
        sky->setSky(Chicane::Box::load<Chicane::Box::Sky>("Assets/Editor/Skies/Default.bsky"));
        sky->setEnvironmentIntensity(0.30f);
    }

    void Scene::spawnLights()
    {
        createActor<Studio>();
    }

    void Scene::spawnCharacter()
    {
        Chicane::Application::getInstance().getController()->attachTo(createActor<Character>());
    }

    void Scene::spawnDefaultItem()
    {
        createActor<Item>("Assets/Editor/Drone/Buster.bmsh");
    }

    void Scene::spawnGizmo()
    {
        m_gizmo = createComponent<Gizmo>(GizmoType::Translation);
    }
}