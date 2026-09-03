#include "Editor/UI/Component/Explorer/Item.reflected.hpp"

#include <Chicane/Core/FileSystem/Item.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Grid/Style.hpp>

namespace Editor
{
    ExplorerItem::ExplorerItem(const pugi::xml_node& inNode)
        : Chicane::Grid::Button(inNode),
          isFolder(false),
          isFile(true),
          typeClass("file"),
          selectionState("idle"),
          itemName(Chicane::String::empty()),
          itemPath(Chicane::String::empty()),
          m_item(nullptr),
          m_boundIndex(-1),
          m_slot(Chicane::Vec2::Zero())
    {
        load("Assets/Editor/UI/Components/Explorer/Item.grid", "Assets/Editor/UI/Components/Explorer/Item.decal");
    }

    void ExplorerItem::bind(const Chicane::FileSystem::Item* inItem, int inIndex, const Chicane::Vec2& inSlot)
    {
        m_item       = inItem;
        m_boundIndex = inIndex;
        m_slot       = inSlot;
        m_style.display.setRaw(Chicane::Grid::Style::DISPLAY_TYPE_FLEX);

        refreshState();
    }

    void ExplorerItem::unbind()
    {
        m_item       = nullptr;
        m_boundIndex = -1;
        m_slot       = Chicane::Vec2::Zero();
        m_style.display.setRaw(Chicane::Grid::Style::DISPLAY_TYPE_NONE);

        refreshState();
    }

    int ExplorerItem::boundIndex() const
    {
        return m_boundIndex;
    }

    void ExplorerItem::onTick(float inDeltaTime)
    {
        Chicane::Grid::Button::onTick(inDeltaTime);

        refreshState();
    }

    void ExplorerItem::refreshPosition()
    {
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
        isFolder       = false;
        isFile         = true;
        typeClass      = "file";
        itemName       = Chicane::String::empty();
        itemPath       = Chicane::String::empty();

        if (!m_item)
        {
            return;
        }

        itemName  = m_item->name;
        itemPath  = m_item->path.lexicallyNormal().toString();
        isFolder  = m_item->type == Chicane::FileSystem::ItemType::Folder;
        isFile    = m_item->type == Chicane::FileSystem::ItemType::File;
        typeClass = isFolder ? "folder" : "file";

        const Chicane::String selected = parseText("{{ selectedAssetName }}");
        if (!selected.isEmpty() && selected.equals(m_item->name))
        {
            selectionState = "selected";
        }
    }
}
