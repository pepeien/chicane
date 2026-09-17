#include "Editor/UI/Component/Outliner.reflected.hpp"

#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Core/Window/Event/Type.hpp>
#include <Chicane/Grid/Component.hpp>
#include <Chicane/Grid/Component/Input/Text.hpp>
#include <Chicane/Grid/Component/View.hpp>

#include "Editor/UI/Component/Dock/Header.hpp"
#include "Editor/UI/Prop.hpp"

namespace Editor
{
    Outliner::Outliner(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          outlinerNodes({}),
          m_bShouldFocusRename(false),
          m_bRenameFocused(false)
    {
        import <DockHeader>();

        load("Assets/Editor/UI/Components/Outliner.grid", "Assets/Editor/UI/Components/Outliner.decal");

        Prop::bind(this, NODES_ATTRIBUTE, outlinerNodes);
    }

    bool Outliner::onEvent(const Chicane::WindowEvent& inEvent)
    {
        if (inEvent.type != Chicane::WindowEventType::MouseButtonDown)
        {
            return Chicane::Grid::Container::onEvent(inEvent);
        }

        const Chicane::Input::MouseButtonEvent event =
            *static_cast<Chicane::Input::MouseButtonEvent*>(inEvent.data);
        if (event.button != Chicane::Input::MouseButton::Left)
        {
            return Chicane::Grid::Container::onEvent(inEvent);
        }

        if (event.clicks >= 2)
        {
            if (isItemBodyAt(event.location))
            {
                beginRename();

                return true;
            }

            return Chicane::Grid::Container::onEvent(inEvent);
        }

        if (hasEditingNode())
        {
            Chicane::Grid::Component* input = findRenameInput();
            if (!input || !input->containsPoint(event.location))
            {
                commitRename();
            }
        }

        return Chicane::Grid::Container::onEvent(inEvent);
    }

    void Outliner::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        Prop::copy(this, NODES_ATTRIBUTE, outlinerNodes);

        if (m_bShouldFocusRename)
        {
            focusRenameInput();
        }

        if (!hasEditingNode())
        {
            m_bRenameFocused = false;

            return;
        }

        Chicane::Grid::Component* input    = findRenameInput();
        const bool                bFocused = input && input->isFocused();
        if (bFocused)
        {
            m_bRenameFocused = true;

            return;
        }

        if (m_bRenameFocused)
        {
            commitRename();
        }
    }

    void Outliner::onItemSelection(Chicane::Object* inItem)
    {
        Prop::invoke(this, ON_ITEM_SELECTION_ATTRIBUTE, inItem);
    }

    void Outliner::onItemToggle(Chicane::Object* inItem)
    {
        Prop::invoke(this, ON_ITEM_TOGGLE_ATTRIBUTE, inItem);
    }

    void Outliner::onItemIdInput(Chicane::Object* inItem, Chicane::String inValue)
    {
        Prop::invoke(this, ON_ITEM_ID_INPUT_ATTRIBUTE, inItem, inValue);
    }

    bool Outliner::isItemBodyAt(const Chicane::Vec2& inLocation) const
    {
        for (Chicane::Grid::Component* node = getHitAt(inLocation); node != nullptr && node != this;
             node                           = node->getParent())
        {
            const Chicane::String& className = node->getClassName();
            if (className.contains("outliner__item__caret"))
            {
                return false;
            }

            if (className.contains("outliner__item__body") ||
                (className.contains("outliner__item") && !className.contains("outliner__item__")))
            {
                return true;
            }

            if (node->isRoot())
            {
                break;
            }
        }

        return false;
    }

    Chicane::Grid::Component* Outliner::findRenameInput() const
    {
        for (Chicane::Grid::Component* child : getChildrenFlat())
        {
            if (child && child->getTag().equals(Chicane::Grid::InputText::TAG_ID) && child->isDisplayable() &&
                !child->isCulled())
            {
                return child;
            }
        }

        return nullptr;
    }

    bool Outliner::hasEditingNode() const
    {
        for (const OutlinerNode& entry : outlinerNodes)
        {
            if (entry.bIsEditing)
            {
                return true;
            }
        }

        return false;
    }

    void Outliner::beginRename()
    {
        m_bShouldFocusRename = true;
        m_bRenameFocused     = false;
        Prop::invoke(this, ON_ITEM_EDIT_ATTRIBUTE);
    }

    void Outliner::commitRename()
    {
        m_bShouldFocusRename = false;
        m_bRenameFocused     = false;
        Prop::invoke(this, ON_ITEM_ID_COMMIT_ATTRIBUTE);
    }

    void Outliner::focusRenameInput()
    {
        Chicane::Grid::Component* input = findRenameInput();
        if (!input)
        {
            return;
        }

        Chicane::Grid::View* view = dynamic_cast<Chicane::Grid::View*>(getRoot());
        if (!view)
        {
            return;
        }

        view->focusOn(input);
        m_bShouldFocusRename = false;
        m_bRenameFocused     = true;
    }
}
