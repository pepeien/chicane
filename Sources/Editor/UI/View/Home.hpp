#pragma once

#include <atomic>
#include <unordered_set>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

#include "Editor/UI/Component/Attributes/CoordinateSpace.hpp"
#include "Editor/UI/Component/Attributes/Field.hpp"
#include "Editor/UI/Component/Attributes/Group.hpp"
#include "Editor/UI/Component/Outliner/Node.hpp"

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::HomeView))
    class HomeView : public Chicane::Grid::View
    {
    public:
        static constexpr inline const char* WORKSPACE_VIEWPORT = "viewport";
        static constexpr inline const char* WORKSPACE_ASSETS   = "assets";
        static constexpr inline const char* STATE_ACTIVE       = "active";
        static constexpr inline const char* STATE_IDLE         = "idle";

    public:
        HomeView();

        void tick(float inDeltaTime) override;

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onAssetImport();

        CH_FUNCTION()
        void onThemeSwitch(Chicane::String inValue);

        CH_FUNCTION()
        void onItemSelection(Chicane::Object* inItem);

        CH_FUNCTION()
        void onItemToggle(Chicane::Object* inItem);

        CH_FUNCTION()
        void onItemEdit();

        CH_FUNCTION()
        void onItemDelete();

        CH_FUNCTION()
        void onItemIdInput(Chicane::Object* inItem, Chicane::String inValue);

        CH_FUNCTION()
        void onItemIdCommit();

        CH_FUNCTION()
        void onWorkspaceViewport();

        CH_FUNCTION()
        void onWorkspaceAssets();

        CH_FUNCTION()
        void onTrackNew();

        CH_FUNCTION()
        void onTrackOpen();

        CH_FUNCTION()
        void onTrackSave();

        CH_FUNCTION()
        void onTrackSaveAs();

        CH_FUNCTION()
        void onAttributeCommit(Chicane::String inName, Chicane::String inValue);

        CH_FUNCTION()
        void onSpawnActor();

        CH_FUNCTION()
        void onSpawnMesh();

        CH_FUNCTION()
        void onSpawn(Chicane::String inTypeName);

        CH_FUNCTION()
        void onGizmoTranslate();

        CH_FUNCTION()
        void onGizmoRotate();

        CH_FUNCTION()
        void onGizmoScale();

        CH_FUNCTION()
        void onExplorerFolder(Chicane::String inPath);

        CH_FUNCTION()
        void onExplorerAsset(Chicane::String inName);

        CH_FUNCTION()
        void onExplorerAssetDrop(Chicane::String inPath);

    private:
        void bindScene();
        void bindScriptEvents();
        void requestOutlinerRebuild();
        void requestAttributesRebuild();
        void flushPendingRebuilds();
        void rebuildOutliner();
        void appendOutlinerNode(
            Chicane::Object* inObject, int inDepth, bool inIsVisible, std::unordered_set<Chicane::Object*>& outLive
        );
        void expandOutlinerAncestors(Chicane::Object* inItem);
        void commitOutlinerEdit(bool bShouldRebuild = true);
        void rebuildAttributes();
        void syncAttributeValues();
        bool hasSelectedItem() const;
        void setWorkspace(const Chicane::String& inValue);

    public:
        CoordinateSpace getCoordinateSpace() const;

    public:
        CH_FIELD()
        OutlinerNode::List outlinerNodes;

        CH_FIELD()
        AttributeField::List attributeFields;

        CH_FIELD()
        AttributeGroup::List attributeGroups;

        CH_FIELD()
        bool bIsItemSelected;
        CH_FIELD()
        Chicane::Object* selectedItem;

        CH_FIELD()
        Chicane::String theme;

        CH_FIELD()
        Chicane::String workspace;
        CH_FIELD()
        bool bIsViewportWorkspace;
        CH_FIELD()
        bool bIsAssetsWorkspace;
        CH_FIELD()
        Chicane::String viewportTabState;
        CH_FIELD()
        Chicane::String assetsTabState;

        CH_FIELD()
        Chicane::String translateState;
        CH_FIELD()
        Chicane::String rotateState;
        CH_FIELD()
        Chicane::String scaleState;

        CH_FIELD()
        Chicane::String selectedFolderPath;
        CH_FIELD()
        Chicane::String selectedAssetName;

    private:
        std::unordered_set<Chicane::Object*> m_collapsedOutlinerItems;
        Chicane::Object*                     m_editingOutlinerItem;
        Chicane::String                      m_outlinerEditId;
        CoordinateSpace                      m_coordinateSpace;
        std::atomic<bool>                    m_bOutlinerDirty;
        std::atomic<bool>                    m_bAttributesDirty;
    };
}
