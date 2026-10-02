#pragma once

#include <atomic>
#include <memory>
#include <unordered_set>

#include <Chicane/Core/Event/Subscription.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

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
        ~HomeView() override;

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
        void onExplorerAssetDrop(Chicane::String inPath);

    public:

    private:
        bool hasSelectedItem() const;

        void bindScene();
        void bindController();
        void unbindController();
        void bindInput(Chicane::Controller* inController);

        void requestOutlinerRebuild();
        void flushPendingRebuilds();
        void rebuildOutliner();

        void appendOutlinerNode(
            Chicane::Object* inObject, int inDepth, bool inIsVisible, std::unordered_set<Chicane::Object*>& outLive
        );
        void expandOutlinerAncestors(Chicane::Object* inItem);
        void syncOutlinerSelection();
        void commitOutlinerEdit(bool bShouldRebuild = true);

        void setWorkspace(const Chicane::String& inValue);

    public:
        CH_FIELD()
        OutlinerNode::List outlinerNodes;

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
        Chicane::String selectedFolderPath;

        CH_FIELD()
        Chicane::String selectedAssetName;

    private:
        Chicane::String                       m_activeWorkspace;
        std::unordered_set<Chicane::Object*>  m_expandedOutlinerItems;
        Chicane::Object*                      m_editingOutlinerItem;
        Chicane::String                       m_outlinerEditId;
        std::atomic<bool>                     m_bOutlinerDirty;
        std::shared_ptr<bool>                 m_bIsListening;
        Chicane::Controller::PawnSubscription m_pawnSubscription;
    };
}
