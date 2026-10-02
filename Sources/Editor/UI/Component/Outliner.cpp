#include "Editor/UI/Component/Outliner.reflected.hpp"

#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Core/Reflection/Type/Field/Acessor.hpp>
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
          m_bShouldFocusRename(false),
          m_bRenameFocused(false)
    {
        import <DockHeader>();

        load("Assets/Editor/UI/Components/Outliner/Index.grid", "Assets/Editor/UI/Components/Outliner/Index.decal");
    }

    bool Outliner::onEvent(const Chicane::WindowEvent& inEvent)
    {
        if (inEvent.type != Chicane::WindowEventType::MouseButtonDown)
        {
            return Chicane::Grid::Container::onEvent(inEvent);
        }

        const Chicane::Input::MouseButtonEvent event = *static_cast<Chicane::Input::MouseButtonEvent*>(inEvent.data);
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

    void Outliner::tick(float inDeltaTime)
    {
        Chicane::Grid::Container::tick(inDeltaTime);

        syncRowSelection();

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

    void Outliner::syncRowSelection()
    {
        Chicane::Object* selected = nullptr;
        for (const Chicane::Grid::Component* node = this; node != nullptr;
             node                                 = node->hasParent() ? node->getParent() : nullptr)
        {
            const Chicane::ReflectionFieldAccessor accessor = node->getField("selectedItem");
            if (accessor.isValid() && accessor.isType<Chicane::Object*>())
            {
                const void* instance =
                    accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(node);
                if (Chicane::Object* const* value = accessor.getValue<Chicane::Object*>(instance))
                {
                    selected = *value;
                }

                break;
            }

            if (node->isRoot())
            {
                break;
            }
        }

        for (Chicane::Grid::Component* row : getChildrenFlat())
        {
            if (!row)
            {
                continue;
            }

            if (!row->classList.contains("outliner__item"))
            {
                continue;
            }

            const Chicane::ReflectionFieldAccessor accessor = row->getField("node");
            if (!accessor.isValid() || !accessor.isType<OutlinerNode>())
            {
                row->setSelected(false);
                continue;
            }

            const void* instance =
                accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(row);
            const OutlinerNode* entry = accessor.getValue<OutlinerNode>(instance);
            row->setSelected(entry && entry->item == selected);
        }
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
        for (const Chicane::Grid::Component* node = this; node != nullptr;
             node                                 = node->hasParent() ? node->getParent() : nullptr)
        {
            const Chicane::ReflectionFieldAccessor accessor = node->getField("outlinerNodes");
            if (accessor.isValid() && accessor.isType<OutlinerNode::List>())
            {
                const void* instance =
                    accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(node);
                if (const OutlinerNode::List* list = accessor.getValue<OutlinerNode::List>(instance))
                {
                    for (const OutlinerNode& entry : *list)
                    {
                        if (entry.bIsEditing)
                        {
                            return true;
                        }
                    }
                }

                return false;
            }

            if (node->isRoot())
            {
                break;
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
