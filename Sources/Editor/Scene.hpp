#pragma once

#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Runtime/Scene.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>

#include "Editor/Component/Gizmo.hpp"

namespace Editor
{
    class Scene : public Chicane::Scene
    {
    public:
        static constexpr inline const char* DEFAULT_TRACK = "Assets/Editor/Levels/Default.track";

    public:
        Scene();

    public:
        void onLoad() override;

    public:
        void setSelection(Chicane::Object* inItem);

        Gizmo* getGizmo() const;
        void setGizmoType(GizmoType inType);

        Chicane::Actor* spawnMeshActor(const Chicane::FileSystem::Path& inMesh);

    protected:
        void spawnLights();
        void spawnCharacter();
        void spawnGizmo();

    private:
        Gizmo* m_gizmo;
    };
}
