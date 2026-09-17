#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Math/Vec/Vec2.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Window/Event.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>
#include <Chicane/Grid/Component/Scrollable.hpp>

#include "Editor/UI/Component/Explorer/Filter.hpp"
#include "Editor/UI/Component/Explorer/TreeEntry.hpp"

namespace Editor
{
    class ExplorerItem;

    CH_TYPE(Type = (Manual), Alias = (Editor::Explorer))
    class Explorer : public Chicane::Grid::Container
    {
    public:
        static constexpr inline const char* ORIENTATION_ATTRIBUTE  = "orientation";
        static constexpr inline const char* ON_FOLDER_ATTRIBUTE    = "onFolder";
        static constexpr inline const char* ON_ASSET_ATTRIBUTE     = "onAsset";
        static constexpr inline const char* ON_ASSET_DROP_ATTRIBUTE = "onAssetDrop";

    private:
        static bool isListedFolder(const Chicane::FileSystem::Item& inItem);
        static bool isListedItem(const Chicane::FileSystem::Item& inItem);
        static Chicane::String toPathKey(const Chicane::FileSystem::Path& inPath);
        static bool hasChildFolders(const Chicane::FileSystem::Item& inItem);

    public:
        CH_CONSTRUCTOR()
        Explorer(const Chicane::XmlNode& inNode);

    public:
        bool onEvent(const Chicane::WindowEvent& inEvent) override;

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onSearch();

        CH_FUNCTION()
        void onToggleFilter();

        CH_FUNCTION()
        void onLayoutHorizontal();
        CH_FUNCTION()
        void onLayoutVertical();

        CH_FUNCTION()
        void onSortMatch();
        CH_FUNCTION()
        void onSortName();

        CH_FUNCTION()
        void onIconSize();

        CH_FUNCTION()
        void onSelectFolder(Chicane::String inPath);

        CH_FUNCTION()
        void onActivateItem(Chicane::String inName);

        void onDropAssetAt(const Chicane::String& inPath, const Chicane::Vec2& inLocation);

        void beginItemDrag(ExplorerItem* inSource, const Chicane::Vec2& inPointer);
        void updateItemDrag(const Chicane::Vec2& inPointer);
        void endItemDrag();

    private:
        void rebuildTree();
        void appendTreeFolders(Chicane::FileSystem::Item::List& inItems, int inDepth);
        void ensureListed(Chicane::FileSystem::Item& inFolder);
        void pumpListings();

        void refreshGrid();
        void syncGridTiles();
        ExplorerItem* createTile();
        Chicane::Grid::Component* findGridContent();

        void refreshToggleStates();

        void refreshFilterLabel();

        void setIconSizeFactor(float inFactor);

        Chicane::Grid::Scrollable* findScrollableAt(const Chicane::Vec2& inLocation) const;
        bool handleWheel(const Chicane::WindowEvent& inEvent);
        bool isOverViewport(const Chicane::Vec2& inLocation) const;
        void ensureDragGhost();
        void showDragGhost(ExplorerItem* inSource, const Chicane::Vec2& inPointer);
        void hideDragGhost();

        const Chicane::FileSystem::Item* findFolder(
            const Chicane::FileSystem::Item& inRoot, const Chicane::String& inPath
        ) const;
        Chicane::FileSystem::Item* findFolder(Chicane::FileSystem::Item& inRoot, const Chicane::String& inPath);
        void expandAncestors(const Chicane::String& inPath);
        bool expandAncestorsFrom(const Chicane::FileSystem::Item& inItem, const Chicane::String& inPath);

    public:
        CH_FIELD()
        Chicane::FileSystem::Item explorerFolder;
        CH_FIELD()
        Chicane::FileSystem::Item::List gridItems;

        CH_FIELD()
        ExplorerTreeEntry::List treeEntries;

        CH_FIELD()
        Chicane::String searchQuery;
        CH_FIELD()
        bool isSearchEmpty;

        CH_FIELD()
        Chicane::String filterLabel;

        CH_FIELD()
        Chicane::String layout;
        CH_FIELD()
        Chicane::String layoutHorizontalState;
        CH_FIELD()
        Chicane::String layoutVerticalState;

        CH_FIELD()
        Chicane::String sortBy;
        CH_FIELD()
        Chicane::String sortMatchState;
        CH_FIELD()
        Chicane::String sortNameState;

        CH_FIELD()
        Chicane::String iconSize;
        CH_FIELD()
        float iconSizePercent;

        CH_FIELD()
        Chicane::String selectedFolderPath;
        CH_FIELD()
        Chicane::String selectedAssetName;

        CH_FIELD()
        Chicane::String orientation;

    private:
        Chicane::FileSystem::Item       m_rootFolder;
        std::unordered_set<std::string> m_expandedPaths;
        std::unordered_set<std::string> m_listedPaths;

        ExplorerFilter m_filter;

        float         m_iconSizeFactor;
        Chicane::Vec2 m_pointer;

        std::vector<ExplorerItem*> m_tiles;
        Chicane::Grid::Component*  m_gridContent;
        Chicane::String            m_gridLayout;
        float                      m_gridIconEm;
        Chicane::XmlDocument       m_tileDocument;

        ExplorerItem*                m_dragGhost;
        ExplorerItem*                m_dragSource;
        Chicane::FileSystem::Item    m_dragItem;
        Chicane::Vec2                m_dragOrigin;
        bool                         m_bDragArmed;
        bool                         m_bDragGhostVisible;
    };
}
