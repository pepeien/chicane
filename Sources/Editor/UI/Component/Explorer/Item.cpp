#include "Editor/UI/Component/Explorer/Item.reflected.hpp"

#include <Chicane/Box.hpp>
#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Box/Asset/Type.hpp>
#include <Chicane/Core/FileSystem/Item.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Core/Input/Mouse/Motion/Event.hpp>
#include <Chicane/Core/Window/Event/Type.hpp>
#include <Chicane/Grid/Style.hpp>
#include <Chicane/Renderer/Draw.hpp>
#include <Chicane/Runtime/Instance.hpp>

#include "Editor/UI/Component/Explorer.hpp"

namespace Editor
{
    ExplorerItem::ExplorerItem(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Button(inNode),
          kind(ExplorerItemKind::File),
          typeClass("file"),
          dragClass("static"),
          ghostClass("solid"),
          selectionState("idle"),
          itemName(Chicane::String::sEmpty()),
          itemPath(Chicane::String::sEmpty()),
          m_item(nullptr),
          m_boundIndex(-1),
          m_slot(Chicane::Vec2::sZero()),
          m_pointer(Chicane::Vec2::sZero()),
          m_grab(Chicane::Vec2::sZero()),
          m_ghostSize(Chicane::Vec2::sZero()),
          m_bIsGhost(false)
    {
        load(
            "Assets/Editor/UI/Components/Explorer/Item/Index.grid",
            "Assets/Editor/UI/Components/Explorer/Item/Index.decal"
        );
    }

    bool ExplorerItem::onEvent(const Chicane::WindowEvent& inEvent)
    {
        if (inEvent.type == Chicane::WindowEventType::MouseMotion)
        {
            const Chicane::Input::MouseMotionEvent event =
                *static_cast<Chicane::Input::MouseMotionEvent*>(inEvent.data);
            m_pointer = event.location;

            if (m_bIsGhost)
            {
                moveGhost(m_pointer);
            }
            else if (isDragging())
            {
                if (Explorer* explorer = findExplorer())
                {
                    explorer->updateItemDrag(m_pointer);
                }
            }
        }
        else if (inEvent.type == Chicane::WindowEventType::MouseButtonDown ||
                 inEvent.type == Chicane::WindowEventType::MouseButtonUp)
        {
            const Chicane::Input::MouseButtonEvent event =
                *static_cast<Chicane::Input::MouseButtonEvent*>(inEvent.data);
            m_pointer = event.location;
        }

        return Chicane::Grid::Button::onEvent(inEvent);
    }

    bool ExplorerItem::escapesOverflow() const
    {
        return m_bIsGhost;
    }

    float ExplorerItem::getDepth() const
    {
        if (m_bIsGhost)
        {
            return 10000.0f;
        }

        return Chicane::Grid::Button::getDepth();
    }

    void ExplorerItem::bind(
        const Chicane::FileSystem::Item* inItem, int inIndex, const Chicane::Vec2& inSlot, bool inShouldRestyle
    )
    {
        if (m_bIsGhost || ghostClass.equals(Chicane::Grid::Style::DISPLAY_TYPE_HIDDEN))
        {
            return;
        }

        const Chicane::String nextPath = inItem ? inItem->path.lexicallyNormal().toString() : Chicane::String::sEmpty();

        if (!inShouldRestyle && itemPath.equals(nextPath) && m_boundIndex == inIndex && m_slot.x == inSlot.x &&
            m_slot.y == inSlot.y)
        {
            return;
        }

        const ExplorerItemKind previousKind = kind;
        const Chicane::String  previousType = typeClass;

        m_item       = nullptr;
        m_boundIndex = inIndex;
        m_slot       = inSlot;
        ghostClass   = "solid";
        itemName     = inItem ? inItem->name : Chicane::String::sEmpty();
        itemPath     = nextPath;
        kind         = (inItem && inItem->type == Chicane::FileSystem::ItemType::Folder) ? ExplorerItemKind::Folder
                                                                                         : ExplorerItemKind::File;
        m_style.display.setRaw(Chicane::Grid::Style::DISPLAY_TYPE_FLEX);

        refreshState();
        refreshPosition();
        markPaintDirty();
        setCulled(false);

        if (inShouldRestyle || previousKind != kind || !previousType.equals(typeClass))
        {
            markStyleDirtySubtree();
            markLayoutDirty();
        }
        else
        {
            markLayoutDirty();
        }
    }

    void ExplorerItem::unbind()
    {
        if (m_bIsGhost || ghostClass.equals(Chicane::Grid::Style::DISPLAY_TYPE_HIDDEN))
        {
            return;
        }

        m_item       = nullptr;
        m_boundIndex = -1;
        m_slot       = Chicane::Vec2::sZero();
        itemName     = Chicane::String::sEmpty();
        itemPath     = Chicane::String::sEmpty();
        kind         = ExplorerItemKind::File;
        m_style.display.setRaw(Chicane::Grid::Style::DISPLAY_TYPE_NONE);

        refreshState();
    }

    int ExplorerItem::boundIndex() const
    {
        return m_boundIndex;
    }

    void ExplorerItem::showGhost(
        const ExplorerItem& inSource, Chicane::FileSystem::Item& ioStorage, const Chicane::Vec2& inPointer
    )
    {
        ioStorage      = {};
        ioStorage.name = inSource.itemName;
        ioStorage.type = inSource.kind == ExplorerItemKind::Folder ? Chicane::FileSystem::ItemType::Folder
                                                                   : Chicane::FileSystem::ItemType::File;

        if (!inSource.itemPath.isEmpty())
        {
            ioStorage.path = Chicane::FileSystem::Path(inSource.itemPath);
            if (ioStorage.type == Chicane::FileSystem::ItemType::File)
            {
                ioStorage.extension = ioStorage.path.extension().toString();
            }
        }

        m_bIsGhost   = true;
        m_item       = &ioStorage;
        m_boundIndex = -2;
        m_slot       = Chicane::Vec2::sZero();
        m_pointer    = inPointer;
        m_grab       = inPointer - inSource.getPosition();
        m_ghostSize  = inSource.getSize();
        ghostClass   = "ghost";

        applyGhostShown();
        refreshState();
        restyleChildren();
        moveGhost(inPointer);
        markPaintDirty();
        setCulled(false);
        markLayoutDirty();
    }

    void ExplorerItem::moveGhost(const Chicane::Vec2& inPointer)
    {
        if (!m_bIsGhost)
        {
            return;
        }

        m_pointer = inPointer;
        setPosition(m_pointer.x - m_grab.x, m_pointer.y - m_grab.y);
        addCursor(m_style.insetLeft(), m_style.insetTop());
        markPaintDirty();
        markLayoutDirty();
    }

    void ExplorerItem::hideGhost()
    {
        const bool bNeedsRestyle = m_bIsGhost || ghostClass.equals("ghost");

        m_bIsGhost   = false;
        m_item       = nullptr;
        m_boundIndex = -1;
        m_grab       = Chicane::Vec2::sZero();
        m_ghostSize  = Chicane::Vec2::sZero();
        ghostClass   = Chicane::Grid::Style::DISPLAY_TYPE_HIDDEN;

        applyGhostHidden();

        if (!bNeedsRestyle)
        {
            return;
        }

        refreshState();
        restyleChildren();
        markPaintDirty();
        markLayoutDirty();
    }

    bool ExplorerItem::isGhost() const
    {
        return m_bIsGhost;
    }

    void ExplorerItem::applyGhostShown()
    {
        if (m_ghostSize.x > 0.0f && m_ghostSize.y > 0.0f)
        {
            setSize(m_ghostSize);
        }

        m_style.display.set(Chicane::Grid::StyleDisplay::Flex);
        m_style.display.setRaw(Chicane::Grid::Style::DISPLAY_TYPE_FLEX);
        m_style.position.set(Chicane::Grid::StylePosition::Absolute);
        m_style.position.setRaw(Chicane::Grid::Style::POSITION_TYPE_ABSOLUTE);
        m_style.opacity.set(0.55f);
        m_style.zIndex.set(999.0f);
    }

    void ExplorerItem::applyGhostHidden()
    {
        m_style.display.set(Chicane::Grid::StyleDisplay::None);
        m_style.display.setRaw(Chicane::Grid::Style::DISPLAY_TYPE_NONE);
        m_style.opacity.set(1.0f);
        m_style.zIndex.set(0.0f);
    }

    void ExplorerItem::restyleChildren()
    {
        for (Chicane::Grid::Component* child : getChildren())
        {
            if (child)
            {
                child->markStyleDirtySubtree();
            }
        }
    }

    void ExplorerItem::onTick(float inDeltaTime)
    {
        Chicane::Grid::Button::onTick(inDeltaTime);

        if (m_bIsGhost)
        {
            const ExplorerItemKind previousKind = kind;
            refreshState();
            if (previousKind != kind)
            {
                restyleChildren();
            }

            applyGhostShown();

            return;
        }

        if (ghostClass.equals(Chicane::Grid::Style::DISPLAY_TYPE_HIDDEN))
        {
            applyGhostHidden();

            return;
        }

        if (m_boundIndex < 0)
        {
            return;
        }

        const ExplorerItemKind previousKind = kind;
        const Chicane::String  previous     = selectionState;
        refreshState();

        if (previousKind != kind || !previous.equals(selectionState))
        {
            markStyleDirtySubtree();
        }
    }

    void ExplorerItem::onDrag()
    {
        if (m_bIsGhost || !isDroppableAsset())
        {
            return;
        }

        if (Explorer* explorer = findExplorer())
        {
            explorer->beginItemDrag(this, m_pointer);
        }
    }

    void ExplorerItem::onDragEnd()
    {
        if (m_bIsGhost)
        {
            return;
        }

        Explorer* explorer = findExplorer();
        if (explorer)
        {
            explorer->endItemDrag();
        }

        if (!isDroppableAsset() || itemPath.isEmpty())
        {
            return;
        }

        if (!explorer)
        {
            return;
        }

        explorer->onDropAssetAt(itemPath, m_pointer);
    }

    void ExplorerItem::refreshPosition()
    {
        if (m_bIsGhost)
        {
            moveGhost(m_pointer);

            return;
        }

        if (m_boundIndex < 0 || !hasParent())
        {
            return;
        }

        m_style.position.setRaw(Chicane::Grid::Style::POSITION_TYPE_ABSOLUTE);

        const Chicane::Grid::Component* parent      = getParent();
        const Chicane::Grid::Style&     parentStyle = parent->getStyle();
        setPosition(
            parent->getPosition().x + parentStyle.insetLeft() + m_slot.x,
            parent->getPosition().y + parentStyle.insetTop() + m_slot.y
        );
        addCursor(m_style.insetLeft(), m_style.insetTop());
    }

    void ExplorerItem::refreshState()
    {
        selectionState = "idle";

        if (m_item)
        {
            itemName = m_item->name;
            itemPath = m_item->path.lexicallyNormal().toString();
            kind     = m_item->type == Chicane::FileSystem::ItemType::Folder ? ExplorerItemKind::Folder
                                                                             : ExplorerItemKind::File;
        }

        typeClass = "file";

        if (kind == ExplorerItemKind::Folder)
        {
            typeClass = "folder";
        }
        else if (!itemPath.isEmpty())
        {
            const Chicane::FileSystem::Path filePath(itemPath);
            const Chicane::Box::AssetType   assetType = Chicane::Box::getTypeFromExtension(filePath);
            if (assetType != Chicane::Box::AssetType::Undefined)
            {
                typeClass = Chicane::toString(assetType).toLower();
            }

            kind = ExplorerItemKind::File;

            if (Chicane::Box::isFileAsset(filePath))
            {
                Chicane::Box::requestPreview(filePath);

                if (const Chicane::Box::AssetPreview* preview = Chicane::Box::findPreview(filePath))
                {
                    Chicane::Instance& application = Chicane::Instance::sInstance();
                    if (application.hasRenderer() && application.getRenderer()->findTexture(preview->textureId()) >
                                                         Chicane::Renderer::Draw::InvalidId)
                    {
                        kind = ExplorerItemKind::Preview;
                    }
                }
            }
        }

        dragClass = isDroppableAsset() ? "draggable" : "static";

        if (m_bIsGhost)
        {
            return;
        }

        const Chicane::String selected = parseText("{{ selectedAssetName }}");
        if (!selected.isEmpty() && selected.equals(itemName))
        {
            selectionState = "selected";
        }
    }

    bool ExplorerItem::isDroppableAsset() const
    {
        if (itemPath.isEmpty())
        {
            return false;
        }

        return Chicane::Box::getTypeFromExtension(Chicane::FileSystem::Path(itemPath)) !=
               Chicane::Box::AssetType::Undefined;
    }

    Explorer* ExplorerItem::findExplorer() const
    {
        for (Chicane::Grid::Component* node = getParent(); node != nullptr; node = node->getParent())
        {
            if (Explorer* explorer = dynamic_cast<Explorer*>(node))
            {
                return explorer;
            }

            if (node->isRoot())
            {
                break;
            }
        }

        return nullptr;
    }
}
