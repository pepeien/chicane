#include "Editor/Viewer/Scene.hpp"

#include <filesystem>

#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Core/Math/Transform.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

namespace Editor
{
    ViewerScene::ViewerScene()
        : Scene(),
          m_asset(),
          m_groups({}),
          m_tempMeshes({})
    {}

    ViewerScene::~ViewerScene()
    {
        for (const Chicane::FileSystem::Path& path : m_tempMeshes)
        {
            std::error_code error;
            std::filesystem::remove(path.toStandard(), error);
        }
    }

    void ViewerScene::onLoad()
    {
        spawnLights();
        spawnCharacter();
        spawnGizmo();
        spawnSlab();
    }

    void ViewerScene::setAsset(const Chicane::FileSystem::Path& inMesh)
    {
        if (m_asset == inMesh)
        {
            return;
        }

        clearPreview();

        m_asset = inMesh;
        if (m_asset.isEmpty() || !Chicane::FileSystem::exists(m_asset))
        {
            return;
        }

        Chicane::Box::Mesh mesh(m_asset);
        for (const Chicane::Box::MeshGroup& group : mesh.getGroups())
        {
            if (!group.isValid())
            {
                continue;
            }

            const Chicane::FileSystem::Path slice = writeGroupMesh(group);
            if (slice.isEmpty())
            {
                continue;
            }

            Chicane::Actor* actor = createActor<Chicane::Actor>();
            actor->setId(group.getId());

            Chicane::CMesh* preview = createComponent<Chicane::CMesh>();
            preview->setIsTransient(true);
            preview->setMesh(slice);
            preview->attachTo(actor);
            preview->activate();

            actor->setAbsolute(group.getTransform());

            m_groups.push_back(actor);
            m_tempMeshes.push_back(slice);
        }
    }

    void ViewerScene::clearAsset()
    {
        clearPreview();
        m_asset = {};
    }

    void ViewerScene::commitGroups()
    {
        if (m_asset.isEmpty() || !Chicane::FileSystem::exists(m_asset))
        {
            return;
        }

        Chicane::Box::Mesh mesh(m_asset);
        for (Chicane::Actor* actor : m_groups)
        {
            if (!actor)
            {
                continue;
            }

            for (Chicane::Box::MeshGroup group : mesh.getGroups())
            {
                if (!group.getId().equals(actor->getId()))
                {
                    continue;
                }

                Chicane::Transform transform;
                transform.setTranslation(actor->getAbsoluteTranslation());
                transform.setRotation(actor->getAbsoluteRotation());
                transform.setScale(actor->getAbsoluteScale());
                group.setTransform(transform);
                mesh.updateGroup(group);

                break;
            }
        }

        mesh.saveXML();
    }

    const Chicane::FileSystem::Path& ViewerScene::getAsset() const
    {
        return m_asset;
    }

    const std::vector<Chicane::Actor*>& ViewerScene::getGroups() const
    {
        return m_groups;
    }

    void ViewerScene::spawnSlab()
    {
        Chicane::Actor* slab = createActor<Chicane::Actor>();
        slab->setIsTransient(true);

        Chicane::CMesh* mesh = createComponent<Chicane::CMesh>();
        mesh->setIsTransient(true);
        mesh->setMesh(Chicane::Box::Mesh::DEFAULT_SOURCE);
        mesh->attachTo(slab);
        mesh->activate();

        constexpr float size   = 8.0f;
        constexpr float height = 0.2f;
        slab->setAbsoluteScale(size, size, height);
        slab->setAbsoluteTranslation(0.0f, 0.0f, -height * 0.5f);
    }

    void ViewerScene::clearPreview()
    {
        setSelection(nullptr);

        for (Chicane::Actor* actor : m_groups)
        {
            if (!actor)
            {
                continue;
            }

            const std::vector<Chicane::Component*> attachments = actor->getAttachments();
            for (Chicane::Component* component : attachments)
            {
                if (!component)
                {
                    continue;
                }

                component->detach();
                removeComponent(component);
                delete component;
            }

            removeActor(actor);
            delete actor;
        }

        m_groups.clear();

        for (const Chicane::FileSystem::Path& path : m_tempMeshes)
        {
            std::error_code error;
            std::filesystem::remove(path.toStandard(), error);
        }

        m_tempMeshes.clear();
        m_asset = {};
    }

    Chicane::FileSystem::Path ViewerScene::writeGroupMesh(const Chicane::Box::MeshGroup& inGroup) const
    {
        std::error_code             error;
        const std::filesystem::path directory = std::filesystem::temp_directory_path() / "chicane-viewer";
        std::filesystem::create_directories(directory, error);
        if (error)
        {
            return {};
        }

        const Chicane::FileSystem::Path path =
            Chicane::FileSystem::Path(directory) /
            (m_asset.stem().toString() + "_" + inGroup.getId() + Chicane::Box::Mesh::EXTENSION);

        Chicane::Box::MeshGroup slice = inGroup;
        slice.setTransform(Chicane::Transform());

        Chicane::Box::Mesh mesh(path);
        mesh.setId(inGroup.getId());
        mesh.setGroups({slice});
        mesh.saveXML();

        return path;
    }
}
