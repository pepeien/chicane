#pragma once

#include <unordered_map>
#include <vector>

#include <Chicane/Core/Event/Subscription.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Runtime/Scene.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

#include "Editor/Component/Gizmo.hpp"
#include "Editor/Scene/Helper.hpp"

namespace Editor
{
    class Scene : public Chicane::Scene
    {
    public:
        static constexpr inline const char* DEFAULT_SCRIPT = "Assets/Editor/Scenes/Default.track";

    public:
        Scene();
        ~Scene() override;

    public:
        void onLoad() override;
        void onTick(float inDeltaTime) override;

    public:
        virtual void destroyObject(Chicane::Object* inObject);

    public:
        void setSelection(Chicane::Object* inItem);
        Chicane::Object* pickObject(const Chicane::SceneTraceRequest& inRequest) const;

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
        void applySelection();
        void updateSelectionOutline();
        void collectSelectionMeshes(Chicane::Object* inItem, std::vector<Chicane::CMesh*>& outMeshes) const;
        bool selectionUsesHelpers(const Chicane::Object* inItem) const;

        void syncHelpers();
        void poseHelper(Chicane::Object* inTarget);
        void poseHelper(Chicane::CMesh* inMesh, Chicane::Object* inTarget);
        Chicane::CMesh* createHelper(const Chicane::FileSystem::Path& inMesh);

        void pushLightTraces();

        bool shouldVisualize(const Chicane::Component* inComponent) const;
        bool isSelectedVisual(const Chicane::Object* inTarget) const;
        bool helperBelongsTo(const Chicane::Object* inTarget, const Chicane::Object* inItem) const;
        Chicane::Object* helperTarget(const Chicane::Object* inObject) const;
        Chicane::Object* selectableFromHit(Chicane::Object* inObject) const;

    private:
        Gizmo*           m_gizmo;
        Chicane::Object* m_selected;

        ComponentsSubscription                            m_helperSubscription;
        std::unordered_map<Chicane::Object*, SceneHelper> m_helpers;
        std::vector<Chicane::CMesh*>                      m_outlined;
        bool                                              m_bSyncingHelpers;
        bool                                              m_bSelectionUsesHelpers;
    };
}
