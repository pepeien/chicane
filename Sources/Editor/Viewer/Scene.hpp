#pragma once

#include <vector>

#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>

#include "Editor/Scene.hpp"

namespace Editor
{
    class ViewerScene : public Scene
    {
    public:
        static constexpr inline const char* NAME = "Viewer Scene";

        static constexpr inline const char* PREVIEW_SHAPE_SHADER_BALL = "Shader Ball";
        static constexpr inline const char* PREVIEW_SHAPE_SPHERE      = "Sphere";
        static constexpr inline const char* PREVIEW_SHAPE_TORUS       = "Torus";
        static constexpr inline const char* PREVIEW_SHAPE_CUBE        = "Cube";
        static constexpr inline const char* PREVIEW_SHAPE_KNOB        = "Small Knob";

        static constexpr inline const char* PREVIEW_MODEL_SHADER_BALL = "Assets/Editor/Models/Preview/ShaderBall.bmdl";
        static constexpr inline const char* PREVIEW_MODEL_SPHERE      = "Assets/Engine/Models/Shape/Sphere.bmdl";
        static constexpr inline const char* PREVIEW_MODEL_TORUS       = "Assets/Engine/Models/Shape/Torus.bmdl";
        static constexpr inline const char* PREVIEW_MODEL_CUBE        = "Assets/Engine/Models/Shape/Cube.bmdl";
        static constexpr inline const char* PREVIEW_MODEL_KNOB        = "Assets/Editor/Models/Preview/Knob.bmdl";

    public:
        ViewerScene();
        ~ViewerScene() override;

    public:
        void onLoad() override;

    public:
        void setAsset(const Chicane::FileSystem::Path& inMesh);
        void setPreviewShape(const Chicane::String& inShape);
        void clearAsset();
        void commitGroups();
        void destroyObject(Chicane::Object* inObject) override;

        const Chicane::FileSystem::Path& getAsset() const;
        const Chicane::String& getPreviewShape() const;
        const std::vector<Chicane::Actor*>& getGroups() const;

    private:
        void loadPreviewStage(Chicane::Box::AssetType inType);
        void frameFromTrack();
        void spawnSky(const Chicane::FileSystem::Path& inSky);
        void orientPreview(Chicane::Actor* inActor);
        Chicane::FileSystem::Path previewTrackPath(Chicane::Box::AssetType inType) const;
        void spawnModel(const Chicane::FileSystem::Path& inModel);
        void spawnPreview();
        void spawnPreviewActor(
            const Chicane::FileSystem::Path&            inMesh,
            const Chicane::String&                      inId,
            const std::vector<Chicane::Box::MeshGroup>& inGroups,
            bool                                        bGroups
        );
        void appendOutline(
            Chicane::Object* inParent, const std::vector<Chicane::Box::MeshGroup>& inGroups, bool bGroups
        );
        Chicane::String uniqueLabel(const Chicane::String& inBase) const;
        void clearPreview();
        void removeTempFiles(std::vector<Chicane::FileSystem::Path>& outFiles);
        const char* previewModelSource() const;
        Chicane::FileSystem::Path writeGroupMesh(
            const Chicane::Box::Mesh& inSource, const Chicane::Box::MeshGroup& inGroup
        ) const;
        Chicane::FileSystem::Path writeTempMesh(
            const Chicane::String& inId, const std::vector<Chicane::Box::MeshGroup>& inGroups
        ) const;

    private:
        Chicane::FileSystem::Path              m_asset;
        Chicane::Box::AssetType                m_previewType;
        Chicane::String                        m_previewShape;
        std::vector<Chicane::Actor*>           m_groups;
        std::vector<Chicane::FileSystem::Path> m_tempMeshes;
        std::vector<Chicane::FileSystem::Path> m_tempFiles;
    };
}
