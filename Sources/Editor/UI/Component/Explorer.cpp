#include "Editor/UI/Component/Explorer.reflected.hpp"

#include <algorithm>
#include <cmath>

#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Core/Input/Mouse/Motion/Event.hpp>
#include <Chicane/Core/Input/Mouse/Wheel/Event.hpp>
#include <Chicane/Core/Window/Event/Type.hpp>
#include <Chicane/Grid/Component.hpp>
#include <Chicane/Grid/Component/Scrollable.hpp>

#include "Editor/UI/Component/Dock/Header.hpp"
#include "Editor/UI/Component/Explorer/Item.hpp"

namespace Editor
{
    constexpr float kIconSizeMinEm = 4.5f;
    constexpr float kIconSizeMaxEm = 10.5f;
    constexpr float kTreeIndentEm  = 0.85f;

    const Chicane::String kLayoutHorizontal = "horizontal";
    const Chicane::String kLayoutVertical   = "vertical";
    const Chicane::String kSortMatch        = "match";
    const Chicane::String kSortName         = "name";
    const Chicane::String kStateActive      = "active";
    const Chicane::String kStateIdle        = "idle";
    const Chicane::String kExpandLeaf       = "leaf";
    const Chicane::String kExpandCollapsed  = "collapsed";
    const Chicane::String kExpandExpanded   = "expanded";
    const Chicane::String kSelected         = "selected";
    const Chicane::String kSliderId         = "explorerIconSize";

    Explorer::Explorer(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          explorerFolder({}),
          gridItems({}),
          treeEntries({}),
          searchQuery(Chicane::String::empty()),
          isSearchEmpty(true),
          layout(kLayoutHorizontal),
          sortBy(kSortMatch),
          filterLabel("All Resources"),
          iconSize("7em"),
          iconSizePercent(40.0f),
          selectedFolderPath(Chicane::String::empty()),
          selectedAssetName(Chicane::String::empty()),
          layoutHorizontalState(kStateActive),
          layoutVerticalState(kStateIdle),
          sortMatchState(kStateActive),
          sortNameState(kStateIdle),
          m_rootFolder({}),
          m_expandedPaths({}),
          m_listedPaths({}),
          m_filter(Filter::All),
          m_iconSizeFactor(0.4f),
          m_bSizing(false),
          m_pointer(Chicane::Vec2::Zero())
    {
        import <DockHeader>();
        import <ExplorerItem>();

        load("Assets/Editor/UI/Components/Explorer.grid", "Assets/Editor/UI/Components/Explorer.decal");

        m_rootFolder = Chicane::FileSystem::Item(Chicane::FileSystem::ItemType::Folder, ".");
        ensureListed(m_rootFolder);
        explorerFolder = m_rootFolder;

        setIconSizeFactor(m_iconSizeFactor);
        rebuildTree();
        refreshGrid();
        refreshToggleStates();
    }

    bool Explorer::onEvent(const Chicane::WindowEvent& inEvent)
    {
        if (inEvent.type == Chicane::WindowEventType::MouseButtonUp)
        {
            if (m_bSizing)
            {
                m_bSizing = false;

                return true;
            }

            return Chicane::Grid::Container::onEvent(inEvent);
        }

        if (inEvent.type == Chicane::WindowEventType::MouseButtonDown)
        {
            const Chicane::Input::MouseButtonEvent event =
                *static_cast<Chicane::Input::MouseButtonEvent*>(inEvent.data);
            if (event.button != Chicane::Input::MouseButton::Left)
            {
                return Chicane::Grid::Container::onEvent(inEvent);
            }

            Chicane::Grid::Component* slider = findSlider();
            if (slider && slider->containsPoint(event.location))
            {
                m_bSizing = true;
                applyIconSizeAt(event.location);

                return true;
            }

            if (event.clicks >= 2)
            {
                for (Chicane::Grid::Component* node = getHitAt(event.location); node != nullptr && node != this;
                     node                           = node->getParent())
                {
                    ExplorerItem* tile = dynamic_cast<ExplorerItem*>(node);
                    if (!tile || !tile->isFolder || tile->itemPath.isEmpty())
                    {
                        continue;
                    }

                    expandAncestors(tile->itemPath);
                    m_expandedPaths.insert(toPathKey(explorerFolder.path).toStandard());
                    onSelectFolder(tile->itemPath);

                    return true;
                }
            }

            return Chicane::Grid::Container::onEvent(inEvent);
        }

        if (inEvent.type == Chicane::WindowEventType::MouseMotion)
        {
            const Chicane::Input::MouseMotionEvent event =
                *static_cast<Chicane::Input::MouseMotionEvent*>(inEvent.data);
            m_pointer = event.location;

            if (!m_bSizing)
            {
                return Chicane::Grid::Container::onEvent(inEvent);
            }

            applyIconSizeAt(event.location);

            return true;
        }

        if (inEvent.type == Chicane::WindowEventType::MouseWheel)
        {
            return handleWheel(inEvent);
        }

        return Chicane::Grid::Container::onEvent(inEvent);
    }

    void Explorer::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        isSearchEmpty = searchQuery.isEmpty();
    }

    void Explorer::onSearch()
    {
        isSearchEmpty = searchQuery.isEmpty();
        refreshGrid();
    }

    void Explorer::onToggleFilter()
    {
        switch (m_filter)
        {
        case Filter::All:
            m_filter = Filter::Folders;
            break;
        case Filter::Folders:
            m_filter = Filter::Files;
            break;
        default:
            m_filter = Filter::All;
            break;
        }

        refreshFilterLabel();
        refreshGrid();
    }

    void Explorer::onLayoutHorizontal()
    {
        layout = kLayoutHorizontal;
        refreshToggleStates();
    }

    void Explorer::onLayoutVertical()
    {
        layout = kLayoutVertical;
        refreshToggleStates();
    }

    void Explorer::onSortMatch()
    {
        sortBy = kSortMatch;
        refreshToggleStates();
        refreshGrid();
    }

    void Explorer::onSortName()
    {
        sortBy = kSortName;
        refreshToggleStates();
        refreshGrid();
    }

    void Explorer::onSelectFolder(Chicane::String inPath)
    {
        if (inPath.isEmpty())
        {
            return;
        }

        inPath = toPathKey(Chicane::FileSystem::Path(inPath));

        Chicane::FileSystem::Item* found = findFolder(m_rootFolder, inPath);
        if (!found)
        {
            return;
        }

        ensureListed(*found);

        explorerFolder     = *found;
        selectedFolderPath = toPathKey(found->path);
        selectedAssetName  = Chicane::String::empty();

        rebuildTree();
        refreshGrid();
    }

    void Explorer::onToggleFolder(Chicane::String inPath)
    {
        if (inPath.isEmpty())
        {
            return;
        }

        inPath = toPathKey(Chicane::FileSystem::Path(inPath));

        Chicane::FileSystem::Item* found = findFolder(m_rootFolder, inPath);
        if (!found)
        {
            return;
        }

        ensureListed(*found);

        if (!hasChildFolders(*found))
        {
            return;
        }

        const std::string key = inPath.toStandard();
        if (m_expandedPaths.erase(key) == 0)
        {
            m_expandedPaths.insert(key);
        }

        rebuildTree();
    }

    void Explorer::onActivateItem(Chicane::String inName)
    {
        selectedAssetName = inName;
    }

    bool Explorer::isListedFolder(const Chicane::FileSystem::Item& inItem)
    {
        return inItem.type == Chicane::FileSystem::ItemType::Folder && isListedItem(inItem);
    }

    bool Explorer::isListedItem(const Chicane::FileSystem::Item& inItem)
    {
        if (inItem.name.isEmpty() || inItem.name.equals(".", ".."))
        {
            return false;
        }

        return !inItem.name.startsWith('.');
    }

    Chicane::String Explorer::toPathKey(const Chicane::FileSystem::Path& inPath)
    {
        return inPath.lexicallyNormal().toString();
    }

    bool Explorer::hasChildFolders(const Chicane::FileSystem::Item& inItem)
    {
        for (const Chicane::FileSystem::Item& child : inItem.children)
        {
            if (isListedFolder(child))
            {
                return true;
            }
        }

        return false;
    }

    void Explorer::rebuildTree()
    {
        treeEntries.clear();
        ensureListed(m_rootFolder);
        appendTreeFolders(m_rootFolder.children, 0);
    }

    void Explorer::appendTreeFolders(Chicane::FileSystem::Item::List& inItems, int inDepth)
    {
        std::vector<std::size_t> folders;
        folders.reserve(inItems.size());

        for (std::size_t i = 0; i < inItems.size(); ++i)
        {
            if (isListedFolder(inItems.at(i)))
            {
                folders.push_back(i);
            }
        }

        std::sort(
            folders.begin(),
            folders.end(),
            [&](std::size_t inA, std::size_t inB)
            { return inItems.at(inA).name.toLower() < inItems.at(inB).name.toLower(); }
        );

        for (const std::size_t index : folders)
        {
            Chicane::FileSystem::Item& folder = inItems.at(index);
            ensureListed(folder);

            ExplorerTreeEntry entry;
            entry.name          = folder.name;
            entry.path          = toPathKey(folder.path);
            entry.indent        = Chicane::String::sprint("%fem", static_cast<float>(inDepth) * kTreeIndentEm);
            entry.selectedState = entry.path.equals(selectedFolderPath) ? kSelected : kStateIdle;

            const bool bHasChildren = hasChildFolders(folder);
            const bool bExpanded    = m_expandedPaths.find(entry.path.toStandard()) != m_expandedPaths.end();

            if (!bHasChildren)
            {
                entry.expandState = kExpandLeaf;
            }
            else
            {
                entry.expandState = bExpanded ? kExpandExpanded : kExpandCollapsed;
            }

            treeEntries.push_back(entry);

            if (bHasChildren && bExpanded)
            {
                appendTreeFolders(folder.children, inDepth + 1);
            }
        }
    }

    void Explorer::refreshGrid()
    {
        gridItems.clear();

        const Chicane::String query = searchQuery.toLower().trim();

        for (const Chicane::FileSystem::Item& child : explorerFolder.children)
        {
            if (!isListedItem(child))
            {
                continue;
            }

            if (m_filter == Filter::Folders && child.type != Chicane::FileSystem::ItemType::Folder)
            {
                continue;
            }

            if (m_filter == Filter::Files && child.type != Chicane::FileSystem::ItemType::File)
            {
                continue;
            }

            if (!query.isEmpty() && !child.name.toLower().contains(query))
            {
                continue;
            }

            Chicane::FileSystem::Item entry = child;
            entry.children.clear();
            gridItems.push_back(entry);
        }

        std::sort(
            gridItems.begin(),
            gridItems.end(),
            [&](const Chicane::FileSystem::Item& inA, const Chicane::FileSystem::Item& inB)
            {
                if (sortBy.equals(kSortMatch) && inA.type != inB.type)
                {
                    return inA.type == Chicane::FileSystem::ItemType::Folder;
                }

                return inA.name.toLower() < inB.name.toLower();
            }
        );
    }

    void Explorer::refreshToggleStates()
    {
        const bool bHorizontal = layout.equals(kLayoutHorizontal);

        layoutHorizontalState = bHorizontal ? kStateActive : kStateIdle;
        layoutVerticalState   = bHorizontal ? kStateIdle : kStateActive;

        const bool bMatch = sortBy.equals(kSortMatch);

        sortMatchState = bMatch ? kStateActive : kStateIdle;
        sortNameState  = bMatch ? kStateIdle : kStateActive;
    }

    void Explorer::refreshFilterLabel()
    {
        switch (m_filter)
        {
        case Filter::Folders:
            filterLabel = "Folder Resource";
            break;
        case Filter::Files:
            filterLabel = "File Resource";
            break;
        default:
            filterLabel = "All Resources";
            break;
        }
    }

    void Explorer::setIconSizeFactor(float inFactor)
    {
        m_iconSizeFactor = std::clamp(inFactor, 0.0f, 1.0f);
        iconSizePercent  = m_iconSizeFactor * 100.0f;
        iconSize =
            Chicane::String::sprint("%.2fem", kIconSizeMinEm + m_iconSizeFactor * (kIconSizeMaxEm - kIconSizeMinEm));
    }

    void Explorer::applyIconSizeAt(const Chicane::Vec2& inLocation)
    {
        Chicane::Grid::Component* slider = findSlider();
        if (!slider)
        {
            return;
        }

        const Chicane::Bounds2D& bounds = slider->getBounds();
        const float              width  = bounds.right - bounds.left;
        if (width <= 0.0f)
        {
            return;
        }

        setIconSizeFactor((inLocation.x - bounds.left) / width);
    }

    Chicane::Grid::Component* Explorer::findSlider() const
    {
        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (child && child->getId().equals(kSliderId))
            {
                return child;
            }
        }

        return nullptr;
    }

    Chicane::Grid::Scrollable* Explorer::findScrollableAt(const Chicane::Vec2& inLocation) const
    {
        Chicane::Grid::Scrollable* main = nullptr;
        Chicane::Grid::Scrollable* tree = nullptr;

        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (!child)
            {
                continue;
            }

            if (child->getId().equals("explorerMain"))
            {
                main = dynamic_cast<Chicane::Grid::Scrollable*>(child);
            }
            else if (child->getId().equals("explorerTree"))
            {
                tree = dynamic_cast<Chicane::Grid::Scrollable*>(child);
            }
        }

        if (main && main->containsPoint(inLocation))
        {
            return main;
        }

        if (tree && tree->containsPoint(inLocation))
        {
            return tree;
        }

        return nullptr;
    }

    bool Explorer::handleWheel(const Chicane::WindowEvent& inEvent)
    {
        const Chicane::Input::MouseWheelEvent event = *static_cast<Chicane::Input::MouseWheelEvent*>(inEvent.data);

        Chicane::Vec2 location = event.location;
        if (location.x == 0.0f && location.y == 0.0f)
        {
            location = m_pointer;
        }

        Chicane::Grid::Scrollable* target = findScrollableAt(location);
        if (!target)
        {
            return false;
        }

        const float step = std::max(16.0f, getStyle().font.size.get() * 3.0f);
        target->addScroll(-event.delta.x * step, -event.delta.y * step);

        return true;
    }

    void Explorer::ensureListed(Chicane::FileSystem::Item& inFolder)
    {
        if (inFolder.type != Chicane::FileSystem::ItemType::Folder)
        {
            return;
        }

        const std::string key = toPathKey(inFolder.path).toStandard();
        if (m_listedPaths.find(key) != m_listedPaths.end())
        {
            return;
        }

        inFolder.children = Chicane::FileSystem::ls(inFolder.path, 1);
        m_listedPaths.insert(key);
    }

    const Chicane::FileSystem::Item* Explorer::findFolder(
        const Chicane::FileSystem::Item& inRoot, const Chicane::String& inPath
    ) const
    {
        if (toPathKey(inRoot.path).equals(inPath))
        {
            return &inRoot;
        }

        for (const Chicane::FileSystem::Item& child : inRoot.children)
        {
            if (const Chicane::FileSystem::Item* found = findFolder(child, inPath))
            {
                return found;
            }
        }

        return nullptr;
    }

    Chicane::FileSystem::Item* Explorer::findFolder(Chicane::FileSystem::Item& inRoot, const Chicane::String& inPath)
    {
        return const_cast<Chicane::FileSystem::Item*>(
            findFolder(static_cast<const Chicane::FileSystem::Item&>(inRoot), inPath)
        );
    }

    void Explorer::expandAncestors(const Chicane::String& inPath)
    {
        for (const Chicane::FileSystem::Item& child : m_rootFolder.children)
        {
            if (expandAncestorsFrom(child, inPath))
            {
                return;
            }
        }
    }

    bool Explorer::expandAncestorsFrom(const Chicane::FileSystem::Item& inItem, const Chicane::String& inPath)
    {
        if (toPathKey(inItem.path).equals(inPath))
        {
            return true;
        }

        for (const Chicane::FileSystem::Item& child : inItem.children)
        {
            if (!expandAncestorsFrom(child, inPath))
            {
                continue;
            }

            m_expandedPaths.insert(toPathKey(inItem.path).toStandard());

            return true;
        }

        return false;
    }
}
