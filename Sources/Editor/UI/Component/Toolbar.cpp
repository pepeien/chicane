#include "Editor/UI/Component/Toolbar.reflected.hpp"

#include <algorithm>
#include <typeindex>
#include <unordered_set>

#include <Chicane/Core/Input/Mouse/Button.hpp>
#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Core/Reflection/Type/Registry.hpp>
#include <Chicane/Core/Window/Event/Type.hpp>

#include "Editor/UI/Component/Header/Menu.hpp"
#include "Editor/UI/Component/ViewportSettings.hpp"
#include "Editor/UI/Prop.hpp"

namespace Editor
{
    static Chicane::String typeTail(const Chicane::String& inName)
    {
        const std::size_t split = inName.lastOf(':');
        if (split == Chicane::String::npos)
        {
            return inName;
        }

        return inName.substr(split + 1);
    }

    static Chicane::String spawnTag(const Chicane::ReflectionTypeInfo& inType)
    {
        for (const Chicane::String& name : inType.names)
        {
            if (!name.contains("::"))
            {
                return name;
            }
        }

        return typeTail(inType.getName());
    }

    static HeaderMenuItem* findOrCreateMenu(HeaderMenuItem::List& inItems, const Chicane::String& inLabel)
    {
        for (HeaderMenuItem& item : inItems)
        {
            if (item.label.equals(inLabel))
            {
                return &item;
            }
        }

        HeaderMenuItem item = {};
        item.label          = inLabel;
        inItems.push_back(item);

        return &inItems.back();
    }

    static void sortMenuItems(HeaderMenuItem::List& inItems)
    {
        std::sort(
            inItems.begin(),
            inItems.end(),
            [](const HeaderMenuItem& inA, const HeaderMenuItem& inB) { return inA.label < inB.label; }
        );

        for (HeaderMenuItem& item : inItems)
        {
            sortMenuItems(item.children);
        }
    }

    Toolbar::Toolbar(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          orientation(Chicane::String::empty()),
          translateState(Chicane::String::empty()),
          rotateState(Chicane::String::empty()),
          scaleState(Chicane::String::empty()),
          isSettingsOpen(false),
          showSettingsHint(true),
          settingsState("idle"),
          isAddOpen(false),
          showAddHint(true),
          addState("idle"),
          menus({})
    {
        import <ViewportSettings>();
        import <HeaderMenu>();

        load("Assets/Editor/UI/Components/Toolbar.grid", "Assets/Editor/UI/Components/Toolbar.decal");

        initAddMenu();

        Prop::bind(this, ORIENTATION_ATTRIBUTE, orientation);
        Prop::bind(this, TRANSLATE_STATE_ATTRIBUTE, translateState);
        Prop::bind(this, ROTATE_STATE_ATTRIBUTE, rotateState);
        Prop::bind(this, SCALE_STATE_ATTRIBUTE, scaleState);
    }

    bool Toolbar::isFocusable() const
    {
        return true;
    }

    bool Toolbar::escapesOverflow() const
    {
        return true;
    }

    bool Toolbar::onEvent(const Chicane::WindowEvent& inEvent)
    {
        if (!isAddOpen || inEvent.type != Chicane::WindowEventType::MouseButtonUp || !inEvent.data)
        {
            return false;
        }

        const Chicane::Input::MouseButtonEvent event =
            *static_cast<const Chicane::Input::MouseButtonEvent*>(inEvent.data);
        if (event.button != Chicane::Input::MouseButton::Left)
        {
            return false;
        }

        Chicane::Grid::Component* hit = hasRoot() ? getRoot()->getHitAt(event.location) : nullptr;
        for (Chicane::Grid::Component* node = hit; node != nullptr; node = node->getParent())
        {
            if (node == this || node->getId().equals(ADD_ANCHOR_ID))
            {
                return false;
            }

            if (node->isRoot())
            {
                break;
            }
        }

        closeAdd();

        return false;
    }

    void Toolbar::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        Prop::copy(this, ORIENTATION_ATTRIBUTE, orientation);
        Prop::copy(this, TRANSLATE_STATE_ATTRIBUTE, translateState);
        Prop::copy(this, ROTATE_STATE_ATTRIBUTE, rotateState);
        Prop::copy(this, SCALE_STATE_ATTRIBUTE, scaleState);

        settingsState    = isSettingsOpen ? "active" : "idle";
        showSettingsHint = !isSettingsOpen;
        addState         = isAddOpen ? "active" : "idle";
        showAddHint      = !isAddOpen;
    }

    void Toolbar::onBlur()
    {
        closeAdd();
    }

    void Toolbar::onTrackSave()
    {
        Prop::invoke(this, ON_TRACK_SAVE_ATTRIBUTE);
    }

    void Toolbar::onGizmoTranslate()
    {
        Prop::invoke(this, ON_GIZMO_TRANSLATE_ATTRIBUTE);
    }

    void Toolbar::onGizmoRotate()
    {
        Prop::invoke(this, ON_GIZMO_ROTATE_ATTRIBUTE);
    }

    void Toolbar::onGizmoScale()
    {
        Prop::invoke(this, ON_GIZMO_SCALE_ATTRIBUTE);
    }

    void Toolbar::onSettingsToggle()
    {
        closeAdd();
        isSettingsOpen = !isSettingsOpen;
    }

    void Toolbar::closeSettings()
    {
        isSettingsOpen = false;
    }

    void Toolbar::onAddToggle()
    {
        closeSettings();

        if (isAddOpen)
        {
            closeAdd();

            return;
        }

        isAddOpen = true;
    }

    void Toolbar::closeAdd()
    {
        if (!isAddOpen)
        {
            closeAddMenus();

            return;
        }

        isAddOpen = false;
        closeAddMenus();
    }

    void Toolbar::onSpawn(Chicane::String inTypeName)
    {
        closeAdd();
        Prop::invoke(this, ON_SPAWN_ATTRIBUTE, inTypeName);
    }

    void Toolbar::initAddMenu()
    {
        menus.clear();

        std::unordered_set<std::type_index> seen;

        for (const auto& [name, type] : Chicane::ReflectionTypeRegistry::getInstance().getAll())
        {
            if (type.group.isEmpty() || type.constructors.empty())
            {
                continue;
            }

            if (type.typeIndex.has_value() && !seen.insert(type.typeIndex.value()).second)
            {
                continue;
            }

            const std::vector<Chicane::String> path = type.group.split(" | ");
            if (path.empty())
            {
                continue;
            }

            if (path.front().trim().equals("Transform"))
            {
                continue;
            }

            const Chicane::String tag = spawnTag(type);
            if (tag.isEmpty())
            {
                continue;
            }

            HeaderMenuItem::List* level = &menus;
            for (std::size_t i = 0; i < path.size(); i++)
            {
                const Chicane::String label = path[i].trim();
                if (label.isEmpty())
                {
                    continue;
                }

                HeaderMenuItem* node = findOrCreateMenu(*level, label);
                if (i + 1 == path.size())
                {
                    node->action = "onSpawn(\"" + tag + "\")";

                    continue;
                }

                level = &node->children;
            }
        }

        sortMenuItems(menus);
    }

    void Toolbar::closeAddMenus()
    {
        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (HeaderMenu* menu = dynamic_cast<HeaderMenu*>(child))
            {
                menu->close();
            }
        }
    }
}
