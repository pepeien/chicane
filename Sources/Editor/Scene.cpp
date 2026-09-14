#include "Editor/Scene.hpp"

#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Track.hpp>

#include "Editor/Actor/Character.hpp"
#include "Editor/Actor/Studio.hpp"

namespace Editor
{
    Scene::Scene()
        : Chicane::Scene(),
          m_gizmo(nullptr)
    {}

    void Scene::onLoad()
    {
        spawnLights();
        spawnCharacter();
        spawnGizmo();
        open(DEFAULT_TRACK);
    }

    void Scene::setSelection(Chicane::Object* inItem)
    {
        for (Chicane::CMesh* mesh : getComponents<Chicane::CMesh>())
        {
            const bool bSelected = inItem != nullptr && (mesh == inItem || mesh->getParent() == inItem);
            mesh->setIsOutlined(bSelected);
        }

        if (!m_gizmo)
        {
            return;
        }

        m_gizmo->setTarget(inItem);
    }

    Gizmo* Scene::getGizmo() const
    {
        return m_gizmo;
    }

    void Scene::setGizmoType(GizmoType inType)
    {
        if (!m_gizmo)
        {
            return;
        }

        m_gizmo->setType(inType);
    }

    Chicane::Actor* Scene::spawnMeshActor(const Chicane::FileSystem::Path& inMesh)
    {
        Chicane::Actor* actor = createActor<Chicane::Actor>();
        Chicane::CMesh* mesh  = createComponent<Chicane::CMesh>();
        mesh->setMesh(inMesh.isEmpty() ? Chicane::Box::Mesh::DEFAULT_SOURCE : inMesh);
        mesh->attachTo(actor);
        mesh->activate();

        return actor;
    }

    void Scene::spawnLights()
    {
        Studio* studio = createActor<Studio>();
        studio->setIsTransient(true);
    }

    void Scene::spawnCharacter()
    {
        Character* character = createActor<Character>();
        character->setIsTransient(true);
        Chicane::Application::getInstance().getController()->attachTo(character);
    }

    void Scene::spawnGizmo()
    {
        m_gizmo = createComponent<Gizmo>(GizmoType::Translation);
        m_gizmo->setIsTransient(true);
    }
}
