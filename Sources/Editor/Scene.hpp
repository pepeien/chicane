#pragma once

#include <unordered_map>

#include <Chicane/Core/Event/Subscription.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Runtime/Scene.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

#include "Editor/Component/Gizmo.hpp"

namespace Editor
{
    class Scene : public Chicane::Scene
    {
    public:
        static constexpr inline const char* DEFAULT_TRACK = "Assets/Editor/Levels/Default.track";

    public:
        Scene();
        ~Scene() override;

    public:
        void onLoad() override;
        void onTick(float inDeltaTime) override;

    public:
        void setSelection(Chicane::Object* inItem);
        virtual void destroyObject(Chicane::Object* inObject);

        Gizmo* getGizmo() const;
        void setGizmoType(GizmoType inType);

        Chicane::Actor* spawnMeshActor(const Chicane::FileSystem::Path& inMesh);

    protected:
        void spawnLights();
        void spawnCharacter();
        void spawnGizmo();
        void spawnHelpers();

    private:
        void destroyObjectTree(Chicane::Object* inObject);
        void syncHelpers();
        void poseHelper(Chicane::Object* inTarget);
        void poseHelper(Chicane::CMesh* inMesh, Chicane::Object* inTarget);
        void pushLightTraces();
        bool shouldVisualize(const Chicane::Component* inComponent) const;
        bool helperBelongsTo(Chicane::Object* inTarget, const Chicane::Object* inItem) const;
        Chicane::CMesh* createHelper(const Chicane::FileSystem::Path& inMesh);

    private:
        struct Helper
        {
            Chicane::CMesh*              mesh = nullptr;
            Chicane::EventSubscription<> subscription;
        };

    private:
        Gizmo*                                       m_gizmo;
        ComponentsSubscription                       m_helperSubscription;
        std::unordered_map<Chicane::Object*, Helper> m_helpers;
        bool                                         m_bSyncingHelpers;
    };
}
