#pragma once

#include <vector>

#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Box/Mesh.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Runtime/Preview/Service.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>

#include "Editor/Scene.hpp"

namespace Editor
{
    class ViewerScene : public Scene
    {
    public:
        static constexpr inline const char* NAME = "Viewer Scene";

        static constexpr inline const char* PREVIEW_SHAPE_SHADER_BALL = Chicane::PreviewService::SHAPE_SHADER_BALL;
        static constexpr inline const char* PREVIEW_SHAPE_SPHERE      = Chicane::PreviewService::SHAPE_SPHERE;
        static constexpr inline const char* PREVIEW_SHAPE_TORUS       = Chicane::PreviewService::SHAPE_TORUS;
        static constexpr inline const char* PREVIEW_SHAPE_CUBE        = Chicane::PreviewService::SHAPE_CUBE;
        static constexpr inline const char* PREVIEW_SHAPE_KNOB        = "Small Knob";

        static constexpr inline const char* PREVIEW_MODEL_KNOB = "Assets/Editor/Models/Preview/Knob.bmdl";

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
        void showMesh(const Chicane::FileSystem::Path& inMesh);
        void syncGroups();
        void frameCamera();
        void appendOutline(
            Chicane::Object* inParent, const std::vector<Chicane::Box::MeshGroup>& inGroups, bool bGroups
        );
        Chicane::String uniqueLabel(const Chicane::String& inBase) const;

    private:
        Chicane::String              m_previewShape;
        std::vector<Chicane::Actor*> m_groups;
    };
}
