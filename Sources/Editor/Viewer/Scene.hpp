#pragma once

#include <vector>

#include <Chicane/Box/Mesh/Group.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>

#include "Editor/Scene.hpp"

namespace Editor
{
    class ViewerScene : public Scene
    {
    public:
        static constexpr inline const char* NAME = "Viewer Scene";

    public:
        ViewerScene();
        ~ViewerScene() override;

    public:
        void onLoad() override;

    public:
        void setAsset(const Chicane::FileSystem::Path& inMesh);
        void clearAsset();
        void commitGroups();
        void destroyObject(Chicane::Object* inObject) override;

        const Chicane::FileSystem::Path& getAsset() const;
        const std::vector<Chicane::Actor*>& getGroups() const;

    private:
        void spawnSlab();
        void clearPreview();
        Chicane::FileSystem::Path writeGroupMesh(const Chicane::Box::MeshGroup& inGroup) const;

    private:
        Chicane::FileSystem::Path              m_asset;
        std::vector<Chicane::Actor*>           m_groups;
        std::vector<Chicane::FileSystem::Path> m_tempMeshes;
    };
}
