#include "Chicane/Grid/Component/ToolTip.reflected.hpp"

#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Window/Event/Type.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid/Component/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        ToolTip::ToolTip(const XmlNode& inNode)
            : Container(inNode),
              anchorId(String::empty()),
              title(String::empty()),
              hasTitle(false),
              hasContent(false),
              isVisible(false),
              panelState("hint")
        {
            for (const auto& child : inNode.getChildren())
            {
                if (child.isElement())
                {
                    hasContent = true;

                    break;
                }
            }

            panelState = hasContent ? "panel" : "hint";

            load("Assets/Engine/UI/Components/ToolTip.grid", "Assets/Engine/UI/Components/ToolTip.decal");

            watchAttribute(
                ANCHOR_ID_ATTRIBUTE_NAME,
                [this](const String& inValue) { anchorId = parseText(inValue).trim(); }
            );

            watchAttribute(
                TITLE_ATTRIBUTE_NAME,
                [this](const String& inValue)
                {
                    title    = parseText(inValue).trim();
                    hasTitle = !title.isEmpty();
                }
            );
        }

        bool ToolTip::escapesOverflow() const
        {
            return isVisible;
        }

        bool ToolTip::onEvent(const WindowEvent& inEvent)
        {
            if (!isPinned() || !isVisible || inEvent.type != WindowEventType::MouseButtonUp || !inEvent.data)
            {
                return false;
            }

            const Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
            if (event.button != Input::MouseButton::Left)
            {
                return false;
            }

            Component* hit = hasRoot() ? getRoot()->getHitAt(event.location) : nullptr;
            for (Component* node = hit; node != nullptr; node = node->getParent())
            {
                if (node == this)
                {
                    return false;
                }

                if (node->getId().equals(anchorId))
                {
                    return false;
                }

                if (node->isRoot())
                {
                    break;
                }
            }

            dismiss();

            return false;
        }

        void ToolTip::onTick(float inDeltaTime)
        {
            refreshAttributes();
            refreshVisibility();
            Container::onTick(inDeltaTime);
        }

        void ToolTip::refreshPosition()
        {
            refreshVisibility();
            Container::refreshPosition();

            if (!isVisible)
            {
                return;
            }

            Component* anchor = findAnchor();
            if (!anchor)
            {
                return;
            }

            const Vec2 current = getPosition();
            const Vec2 target(anchor->getPosition().x, anchor->getPosition().y + anchor->getSize().y + 6.0f);

            addPosition(target.x - current.x, target.y - current.y);
        }

        void ToolTip::dismiss()
        {
            if (!isVisible)
            {
                return;
            }

            isVisible = false;
            getMethod(getAttribute(ON_CLOSE_ATTRIBUTE_NAME)).invoke();
        }

        void ToolTip::refreshAttributes()
        {
            const Component* context = hasParent() ? getParent() : this;

            const String nextAnchor = context->parseText(getAttribute(ANCHOR_ID_ATTRIBUTE_NAME)).trim();
            if (!nextAnchor.equals(anchorId))
            {
                anchorId = nextAnchor;
            }

            const String nextTitle = context->parseText(getAttribute(TITLE_ATTRIBUTE_NAME)).trim();
            if (!nextTitle.equals(title))
            {
                title = nextTitle;
            }

            hasTitle   = !title.isEmpty();
            panelState = hasContent ? "panel" : "hint";
        }

        void ToolTip::refreshVisibility()
        {
            bool bReveal = false;

            if (isPinned())
            {
                const String raw = getAttribute(IS_OPEN_ATTRIBUTE_NAME);
                const String parsed =
                    hasParent() ? getParent()->parseText(raw).trim() : parseText(raw).trim();

                bReveal = Xml::parseBool(parsed, false);
            }
            else if (hasTitle || hasContent)
            {
                bReveal = isAnchorHovered(findAnchor()) || (isVisible && isHovered());
            }

            if (isVisible == bReveal)
            {
                return;
            }

            isVisible = bReveal;
            markLayoutDirtySubtree();
        }

        bool ToolTip::isPinned() const
        {
            return !getAttribute(IS_OPEN_ATTRIBUTE_NAME).isEmpty();
        }

        static Component* findAnchorIn(Component* inOrigin, const String& inId)
        {
            if (!inOrigin || inId.isEmpty())
            {
                return nullptr;
            }

            if (inOrigin->getId().equals(inId))
            {
                return inOrigin;
            }

            for (Component* child : inOrigin->getChildren())
            {
                if (Component* found = findAnchorIn(child, inId))
                {
                    return found;
                }
            }

            return nullptr;
        }

        Component* ToolTip::findAnchor() const
        {
            if (!anchorId.isEmpty())
            {
                for (Component* origin = getParent(); origin != nullptr; origin = origin->getParent())
                {
                    if (Component* found = findAnchorIn(origin, anchorId))
                    {
                        return found;
                    }

                    if (origin->isRoot())
                    {
                        break;
                    }
                }
            }

            return hasParent() ? getParent() : nullptr;
        }

        bool ToolTip::isAnchorHovered(const Component* inAnchor) const
        {
            if (!inAnchor)
            {
                return false;
            }

            if (inAnchor->isHovered())
            {
                return true;
            }

            const Component* root = getRoot();
            const View*      view = dynamic_cast<const View*>(root ? root : this);
            if (!view)
            {
                return false;
            }

            for (Component* node = view->getHovered(); node != nullptr;
                 node            = node->isRoot() ? nullptr : node->getParent())
            {
                if (node == inAnchor)
                {
                    return true;
                }
            }

            return false;
        }
    }
}
