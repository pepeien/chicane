#pragma once

#include <string>
#include <unordered_set>

#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Math/Vec/Vec2.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Window/Event.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>
#include <Chicane/Grid/Component/Scrollable.hpp>

#include "Editor/UI/Component/Explorer/TreeEntry.hpp"

namespace Editor
{
    CH_TYPE(Manual)
    class Explorer : public Chicane::Grid::Container
    {
    public:
        CH_CONSTRUCTOR()
        Explorer(const pugi::xml_node& inNode);

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
        void onSelectFolder(Chicane::String inPath);

        CH_FUNCTION()
        void onToggleFolder(Chicane::String inPath);

        CH_FUNCTION()
        void onActivateItem(Chicane::String inName);

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
        Chicane::String layout;

        CH_FIELD()
        Chicane::String sortBy;

        CH_FIELD()
        Chicane::String filterLabel;

        CH_FIELD()
        Chicane::String iconSize;

        CH_FIELD()
        float iconSizePercent;

        CH_FIELD()
        Chicane::String selectedFolderPath;

        CH_FIELD()
        Chicane::String selectedAssetName;

        CH_FIELD()
        Chicane::String layoutHorizontalState;

        CH_FIELD()
        Chicane::String layoutVerticalState;

        CH_FIELD()
        Chicane::String sortMatchState;

        CH_FIELD()
        Chicane::String sortNameState;

    private:
        enum class Filter : std::uint8_t
        {
            All,
            Folders,
            Files
        };

        static bool isListedFolder(const Chicane::FileSystem::Item& inItem);
        static bool isListedItem(const Chicane::FileSystem::Item& inItem);
        static Chicane::String toPathKey(const Chicane::FileSystem::Path& inPath);
        static bool hasChildFolders(const Chicane::FileSystem::Item& inItem);

        void rebuildTree();
        void appendTreeFolders(Chicane::FileSystem::Item::List& inItems, int inDepth);
        void refreshGrid();
        void ensureListed(Chicane::FileSystem::Item& inFolder);
        void refreshToggleStates();
        void refreshFilterLabel();
        void setIconSizeFactor(float inFactor);
        void applyIconSizeAt(const Chicane::Vec2& inLocation);
        Chicane::Grid::Component* findSlider() const;
        Chicane::Grid::Scrollable* findScrollableAt(const Chicane::Vec2& inLocation) const;
        bool handleWheel(const Chicane::WindowEvent& inEvent);
        const Chicane::FileSystem::Item* findFolder(
            const Chicane::FileSystem::Item& inRoot,
            const Chicane::String&           inPath
        ) const;
        Chicane::FileSystem::Item* findFolder(
            Chicane::FileSystem::Item& inRoot,
            const Chicane::String&     inPath
        );
        void expandAncestors(const Chicane::String& inPath);
        bool expandAncestorsFrom(const Chicane::FileSystem::Item& inItem, const Chicane::String& inPath);

        Chicane::FileSystem::Item       m_rootFolder;
        std::unordered_set<std::string> m_expandedPaths;
        std::unordered_set<std::string> m_listedPaths;
        Filter                         m_filter;
        float                          m_iconSizeFactor;
        bool                           m_bSizing;
        Chicane::Vec2                  m_pointer;
    };
}
