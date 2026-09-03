#include "Editor/UI/Component/Explorer.reflected.hpp"

#include <algorithm>
#include <cmath>

#include <Chicane/Box/Font.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Core/Input/Mouse/Motion/Event.hpp>
#include <Chicane/Core/Input/Mouse/Wheel/Event.hpp>
#include <Chicane/Core/Window/Event/Type.hpp>
#include <Chicane/Grid/Component.hpp>
#include <Chicane/Grid/Component/Scrollable.hpp>
#include <Chicane/Grid/Style/Display.hpp>

#include "Editor/UI/Component/Dock/Header.hpp"
#include "Editor/UI/Component/Explorer/Item.hpp"

namespace Editor
{
    constexpr float       ICON_SIZE_MIN_EM = 4.5f;
    constexpr float       ICON_SIZE_MAX_EM = 10.5f;
    const Chicane::String SLIDER_ID        = "explorerIconSize";

    constexpr float TREE_INDENT_EM = 0.85f;

    constexpr float       GRID_GAP_EM        = 0.55f;
    constexpr float       LIST_GAP_EM        = 0.2f;
    constexpr float       LIST_ROW_EM        = 2.5f;
    constexpr int         TILE_OVERSCAN_ROWS = 2;
    const Chicane::String GRID_CONTENT_ID    = "explorerGridContent";

    const Chicane::String LAYOUT_HORIZONTAL = "horizontal";
    const Chicane::String LAYOUT_VERTICAL   = "vertical";

    const Chicane::String SORT_MATCH = "match";
    const Chicane::String SORT_NAME  = "name";

    const Chicane::String STATE_ACTIVE = "active";
    const Chicane::String STATE_IDLE   = "idle";

    const Chicane::String EXPAND_LEAF      = "leaf";
    const Chicane::String EXPAND_COLLAPSED = "collapsed";
    const Chicane::String EXPAND_EXPANDED  = "expanded";

    const Chicane::String SELECTED = "selected";

    Explorer::Explorer(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          explorerFolder({}),
          gridItems({}),
          treeEntries({}),
          searchQuery(Chicane::String::empty()),
          isSearchEmpty(true),
          filterLabel("All Resources"),
          layout(LAYOUT_HORIZONTAL),
          layoutHorizontalState(STATE_ACTIVE),
          layoutVerticalState(STATE_IDLE),
          sortBy(SORT_MATCH),
          sortMatchState(STATE_ACTIVE),
          sortNameState(STATE_IDLE),
          iconSize("7em"),
          iconSizePercent(40.0f),
          selectedFolderPath(Chicane::String::empty()),
          selectedAssetName(Chicane::String::empty()),
          m_rootFolder({}),
          m_expandedPaths({}),
          m_listedPaths({}),
          m_filter(ExplorerFilter::All),
          m_iconSizeFactor(0.4f),
          m_bSizing(false),
          m_pointer(Chicane::Vec2::Zero()),
          m_tiles({}),
          m_gridContent(nullptr),
          m_gridLayout(Chicane::String::empty())
    {
        import <DockHeader>();
        import <ExplorerItem>();

        load("Assets/Editor/UI/Components/Explorer.grid", "Assets/Editor/UI/Components/Explorer.decal");

        pugi::xml_node tile = m_tileDocument.append_child(ExplorerItem::TAG_ID);
        tile.append_attribute("class").set_value("--{{ layout }}");
        tile.append_attribute("onClick").set_value("{{ onActivateItem(itemName) }}");

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
                    if (!node->getTag().equals(ExplorerItem::TAG_ID))
                    {
                        continue;
                    }

                    ExplorerItem* tile = static_cast<ExplorerItem*>(node);
                    if (!tile->isFolder || tile->itemPath.isEmpty())
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

        syncGridTiles();
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
        case ExplorerFilter::All:
            m_filter = ExplorerFilter::Folders;
            break;
        case ExplorerFilter::Folders:
            m_filter = ExplorerFilter::Files;
            break;
        default:
            m_filter = ExplorerFilter::All;
            break;
        }

        refreshFilterLabel();
        refreshGrid();
    }

    void Explorer::onLayoutHorizontal()
    {
        layout = LAYOUT_HORIZONTAL;

        refreshToggleStates();
    }

    void Explorer::onLayoutVertical()
    {
        layout = LAYOUT_VERTICAL;

        refreshToggleStates();
    }

    void Explorer::onSortMatch()
    {
        sortBy = SORT_MATCH;

        refreshToggleStates();
        refreshGrid();
    }

    void Explorer::onSortName()
    {
        sortBy = SORT_NAME;

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
            entry.indent        = Chicane::String::sprint("%fem", static_cast<float>(inDepth) * TREE_INDENT_EM);
            entry.selectedState = entry.path.equals(selectedFolderPath) ? SELECTED : STATE_IDLE;

            const bool bHasChildren = hasChildFolders(folder);
            const bool bExpanded    = m_expandedPaths.find(entry.path.toStandard()) != m_expandedPaths.end();

            if (!bHasChildren)
            {
                entry.expandState = EXPAND_LEAF;
            }
            else
            {
                entry.expandState = bExpanded ? EXPAND_EXPANDED : EXPAND_COLLAPSED;
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

            if (m_filter == ExplorerFilter::Folders && child.type != Chicane::FileSystem::ItemType::Folder)
            {
                continue;
            }

            if (m_filter == ExplorerFilter::Files && child.type != Chicane::FileSystem::ItemType::File)
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
                if (sortBy.equals(SORT_MATCH) && inA.type != inB.type)
                {
                    return inA.type == Chicane::FileSystem::ItemType::Folder;
                }

                return inA.name.toLower() < inB.name.toLower();
            }
        );
    }

    void Explorer::refreshToggleStates()
    {
        const bool bHorizontal = layout.equals(LAYOUT_HORIZONTAL);

        layoutHorizontalState = bHorizontal ? STATE_ACTIVE : STATE_IDLE;
        layoutVerticalState   = bHorizontal ? STATE_IDLE : STATE_ACTIVE;

        const bool bMatch = sortBy.equals(SORT_MATCH);

        sortMatchState = bMatch ? STATE_ACTIVE : STATE_IDLE;
        sortNameState  = bMatch ? STATE_IDLE : STATE_ACTIVE;
    }

    void Explorer::refreshFilterLabel()
    {
        switch (m_filter)
        {
        case ExplorerFilter::Folders:
            filterLabel = "Folder Resource";
            break;
        case ExplorerFilter::Files:
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
        iconSize         = Chicane::String::sprint(
            "%.2fem",
            ICON_SIZE_MIN_EM + m_iconSizeFactor * (ICON_SIZE_MAX_EM - ICON_SIZE_MIN_EM)
        );
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
            if (child && child->getId().equals(SLIDER_ID))
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
        Chicane::Grid::Scrollable* grid = nullptr;

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
            else if (child->getId().equals(GRID_CONTENT_ID))
            {
                grid = dynamic_cast<Chicane::Grid::Scrollable*>(child);
            }
        }

        if (grid && grid->containsPoint(inLocation))
        {
            return grid;
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

    void Explorer::syncGridTiles()
    {
        Chicane::Grid::Component* content = findGridContent();
        if (!content)
        {
            return;
        }

        Chicane::Grid::Scrollable* scrollable = dynamic_cast<Chicane::Grid::Scrollable*>(content);
        const std::size_t          count      = gridItems.size();

        if (count == 0)
        {
            while (!m_tiles.empty())
            {
                ExplorerItem* extra = m_tiles.back();
                m_tiles.pop_back();
                content->removeChild(extra);
            }

            if (scrollable)
            {
                scrollable->clearVirtualContentSize();
            }

            return;
        }

        const bool  bIsVertical       = layout.equals(LAYOUT_VERTICAL);
        const bool  bHasLayoutChanged = !layout.equals(m_gridLayout);
        const auto& style             = content->getStyle();
        const float available         = std::max(0.0f, content->getSize().x - style.insetHorizontal());
        const float viewH             = std::max(0.0f, content->getSize().y - style.insetVertical());
        const float em     = style.font.size.get() > 0.0f ? style.font.size.get() : Chicane::Box::Font::BASE_SIZE;
        const float iconEm = ICON_SIZE_MIN_EM + m_iconSizeFactor * (ICON_SIZE_MAX_EM - ICON_SIZE_MIN_EM);
        const float gapX   = bIsVertical ? 0.0f : GRID_GAP_EM * em;
        const float gapY   = (bIsVertical ? LIST_GAP_EM : GRID_GAP_EM) * em;
        const float cellW  = bIsVertical ? std::max(available, 1.0f) : iconEm * em;
        const float cellH  = bIsVertical ? LIST_ROW_EM * em : cellW;

        m_gridLayout = layout;

        const float strideX = std::max(cellW + gapX, 1.0f);
        const float strideY = std::max(cellH + gapY, 1.0f);

        std::size_t columns = 1;
        if (!bIsVertical && available > 0.0f)
        {
            columns = std::max(static_cast<std::size_t>(1), static_cast<std::size_t>(available / strideX));
        }

        const std::size_t rows = (count + columns - 1) / columns;
        const float       contentW =
            bIsVertical ? available : (columns * cellW + (columns > 0 ? (columns - 1) * gapX : 0.0f));
        const float contentH = rows * cellH + (rows > 0 ? (rows - 1) * gapY : 0.0f);

        if (scrollable)
        {
            scrollable->setVirtualContentSize(Chicane::Vec2(std::max(contentW, available), contentH));
            scrollable->setScroll(bHasLayoutChanged ? Chicane::Vec2::Zero() : scrollable->getScroll());
        }

        const float scrollY  = scrollable ? scrollable->getScroll().y : 0.0f;
        int         firstRow = static_cast<int>(std::floor(scrollY / strideY)) - TILE_OVERSCAN_ROWS;
        int         lastRow  = static_cast<int>(std::floor((scrollY + viewH) / strideY)) + TILE_OVERSCAN_ROWS;
        const int   maxRow   = static_cast<int>(rows == 0 ? 0 : rows - 1);

        firstRow = std::max(0, firstRow);
        lastRow  = std::min(maxRow, std::max(firstRow, lastRow));

        const std::size_t first  = static_cast<std::size_t>(firstRow) * columns;
        const std::size_t last   = std::min(count - 1, static_cast<std::size_t>(lastRow + 1) * columns - 1);
        const std::size_t window = last >= first ? (last - first + 1) : 0;

        while (m_tiles.size() < window)
        {
            m_tiles.push_back(createTile());
        }

        while (m_tiles.size() > window)
        {
            ExplorerItem* extra = m_tiles.back();
            m_tiles.pop_back();
            content->removeChild(extra);
        }

        for (std::size_t i = 0; i < m_tiles.size(); ++i)
        {
            ExplorerItem* tile = m_tiles.at(i);

            const std::size_t   dataIndex = first + i;
            const std::size_t   column    = dataIndex % columns;
            const std::size_t   row       = dataIndex / columns;
            const Chicane::Vec2 slot =
                bIsVertical ? Chicane::Vec2(0.0f, static_cast<float>(row) * strideY)
                            : Chicane::Vec2(static_cast<float>(column) * strideX, static_cast<float>(row) * strideY);

            const bool bRebind = bHasLayoutChanged || tile->boundIndex() != static_cast<int>(dataIndex);
            tile->bind(&gridItems.at(dataIndex), static_cast<int>(dataIndex), slot);

            if (bRebind)
            {
                tile->markStyleDirtySubtree();
            }
        }
    }

    ExplorerItem* Explorer::createTile()
    {
        Chicane::Grid::Component* content = findGridContent();
        ExplorerItem*             tile    = new ExplorerItem(m_tileDocument.first_child());
        if (content)
        {
            content->addChild(tile);
        }

        return tile;
    }

    Chicane::Grid::Component* Explorer::findGridContent()
    {
        if (m_gridContent)
        {
            return m_gridContent;
        }

        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (child && child->getId().equals(GRID_CONTENT_ID))
            {
                m_gridContent = child;

                break;
            }
        }

        return m_gridContent;
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
