#include "Editor/UI/Component/Header/Menu.reflected.hpp"

#include <Chicane/Grid/Component.hpp>

namespace Editor
{
    HeaderMenu::HeaderMenu(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          bIsOpen(false),
          bShowChevron(false),
          bHasShortcut(false),
          label(Chicane::String::empty()),
          shortcut(Chicane::String::empty()),
          m_bHasSubmenuList(false)
    {
        import <HeaderMenu>();

        load("Assets/Editor/UI/Components/Header/Menu.grid", "Assets/Editor/UI/Components/Header/Menu.decal");
    }

    void HeaderMenu::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        refreshItem();
    }

    void HeaderMenu::onHover()
    {
        if (isNested())
        {
            if (hasSubmenu())
            {
                open();

                return;
            }

            closeSiblings();

            return;
        }

        if (hasOpenSibling())
        {
            open();
        }
    }

    void HeaderMenu::onActivate()
    {
        if (hasSubmenu())
        {
            toggle();

            return;
        }

        const HeaderMenuItem* item = getItem();
        if (item && !item->action.isEmpty())
        {
            getMethod(item->action).invoke();
        }

        closeRoot();
    }

    void HeaderMenu::toggle()
    {
        if (bIsOpen)
        {
            close();

            return;
        }

        open();
    }

    void HeaderMenu::open()
    {
        closeSiblings();

        if (bIsOpen)
        {
            return;
        }

        bIsOpen = true;
        ensureSubmenu();
        bindSubmenu();
        refreshStyleSubtree();
    }

    void HeaderMenu::close()
    {
        if (!bIsOpen)
        {
            for (Chicane::Grid::Component* child : getChildrenFlat())
            {
                if (HeaderMenu* menu = dynamic_cast<HeaderMenu*>(child))
                {
                    menu->close();
                }
            }

            return;
        }

        bIsOpen = false;

        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (HeaderMenu* menu = dynamic_cast<HeaderMenu*>(child))
            {
                menu->close();
            }
        }

        refreshStyleSubtree();
    }

    bool HeaderMenu::hasSubmenu() const
    {
        const HeaderMenuItem* item = getItem();

        return item && !item->children.empty();
    }

    bool HeaderMenu::showChevron() const
    {
        return isNested() && hasSubmenu();
    }

    bool HeaderMenu::hasShortcut() const
    {
        const HeaderMenuItem* item = getItem();

        return item && !item->shortcut.isEmpty();
    }

    bool HeaderMenu::isNested() const
    {
        return findParentMenu() != nullptr;
    }

    bool HeaderMenu::hasOpenSibling() const
    {
        Chicane::Grid::Component* parent = getParent();
        if (!parent)
        {
            return false;
        }

        for (Chicane::Grid::Component* child : parent->getChildren())
        {
            HeaderMenu* menu = dynamic_cast<HeaderMenu*>(child);
            if (menu && menu != this && menu->bIsOpen)
            {
                return true;
            }
        }

        return false;
    }

    const HeaderMenuItem* HeaderMenu::getItem() const
    {
        for (const char* name : {"menu", "child"})
        {
            const Chicane::ReflectionFieldAccessor accessor = getField(name);
            if (!accessor.isValid() || !accessor.isType<HeaderMenuItem>())
            {
                continue;
            }

            const void* instance =
                accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(this);

            if (const HeaderMenuItem* item = accessor.getValue<HeaderMenuItem>(instance))
            {
                return item;
            }
        }

        return nullptr;
    }

    HeaderMenu* HeaderMenu::findParentMenu() const
    {
        Chicane::Grid::Component* node = getParent();
        while (node)
        {
            if (HeaderMenu* menu = dynamic_cast<HeaderMenu*>(node))
            {
                return menu;
            }

            if (node->isRoot())
            {
                break;
            }

            node = node->getParent();
        }

        return nullptr;
    }

    void HeaderMenu::closeSiblings()
    {
        Chicane::Grid::Component* parent = getParent();
        if (!parent)
        {
            return;
        }

        for (Chicane::Grid::Component* child : parent->getChildren())
        {
            HeaderMenu* menu = dynamic_cast<HeaderMenu*>(child);
            if (!menu || menu == this)
            {
                continue;
            }

            menu->close();
        }
    }

    void HeaderMenu::closeRoot()
    {
        HeaderMenu* node = this;
        while (HeaderMenu* parent = node->findParentMenu())
        {
            node = parent;
        }

        node->closeSiblings();
        node->close();
    }

    void HeaderMenu::ensureSubmenu()
    {
        if (m_bHasSubmenuList || !hasSubmenu())
        {
            return;
        }

        m_bHasSubmenuList = true;

        load("Assets/Editor/UI/Components/Header/Menu/List.grid");
        bindSubmenu();
    }

    void HeaderMenu::refreshItem()
    {
        const HeaderMenuItem* item = getItem();

        bShowChevron = isNested() && item && !item->children.empty();
        bHasShortcut = item && !item->shortcut.isEmpty();
        label        = item ? item->label : Chicane::String::empty();
        shortcut     = item ? item->shortcut : Chicane::String::empty();

        if (m_bHasSubmenuList)
        {
            bindSubmenu();
        }
    }

    void HeaderMenu::bindSubmenu()
    {
        Chicane::ReflectionFieldAccessor children = getField("menu.children");
        if (!children.isValid())
        {
            children = getField("child.children");
        }

        if (!children.isValid())
        {
            return;
        }

        addVariable("submenu", children);
    }
}
