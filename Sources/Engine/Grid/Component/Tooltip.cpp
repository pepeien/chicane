#include "Chicane/Grid/Component/Tooltip.reflected.hpp"

#include <algorithm>

#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Window/Event/Type.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid/Component/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        Tooltip::Tooltip(const XmlNode& inNode)
            : Container(inNode),
              anchorId(String::sEmpty()),
              title(String::sEmpty()),
              hasTitle(false),
              hasContent(false),
              isVisible(false),
              panelState("hint"),
              contentDelay(DEFAULT_CONTENT_DELAY_IN_MS)
        {
            for (const XmlNode& child : inNode.getChildren())
            {
                if (child.isElement())
                {
                    hasContent = true;

                    break;
                }
            }

            panelState = hasContent ? "panel" : "hint";

            load("Assets/Engine/UI/Components/Tooltip/Index.grid", "Assets/Engine/UI/Components/Tooltip/Index.decal");

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

        bool Tooltip::onEvent(const WindowEvent& inEvent)
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
            if (containsNode(hit))
            {
                return false;
            }

            for (Component* node = hit; node != nullptr; node = node->getParent())
            {
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

        void Tooltip::onTick(float inDeltaTime)
        {
            refreshAttributes();
            refreshVisibility();

            if (isVisible)
            {
                refreshPosition();
            }

            Container::onTick(inDeltaTime);
        }

        void Tooltip::refreshPosition()
        {
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

            const float marginLeft = style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get();
            const float marginTop  = style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get();

            const Vec2 current = getPosition();
            const Vec2 target(
                anchor->getPosition().x + marginLeft,
                anchor->getPosition().y + anchor->getSize().y + 6.0f + marginTop
            );

            if (current.x != target.x || current.y != target.y)
            {
                addPosition(target.x - current.x, target.y - current.y);
            }

            reflowChildPositions();
        }

        void Tooltip::dismiss()
        {
            if (!isVisible)
            {
                return;
            }

            isVisible = false;
            getMethod(getAttribute(ON_CLOSE_ATTRIBUTE_NAME)).invoke();
        }

        void Tooltip::refreshAttributes()
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

            const String delayRaw  = getAttribute(DESCRIPTION_DELAY_ATTRIBUTE_NAME);
            const String delayText = hasParent() ? getParent()->parseText(delayRaw).trim() : parseText(delayRaw).trim();

            contentDelay = std::max(0.0f, Xml::parseFloat(delayText, DEFAULT_CONTENT_DELAY_IN_MS));
        }

        void Tooltip::refreshVisibility()
        {
            bool bReveal = false;

            const bool bPinned = static_cast<bool>(isPinned());

            if (bPinned)
            {
                const String raw    = getAttribute(IS_OPEN_ATTRIBUTE_NAME);
                const String parsed = hasParent() ? getParent()->parseText(raw).trim() : parseText(raw).trim();

                bReveal = Xml::parseBool(parsed, false);
            }

            const bool bHasTitleOrHasContent = !bPinned && (hasTitle || hasContent);

            if (bHasTitleOrHasContent)
            {
                bReveal = isAnchorHovered(findAnchor()) || (isVisible && isHovered());
            }

            if (isVisible == bReveal)
            {
                return;
            }

            isVisible = bReveal;
            setEscapesOverflow(isVisible);
            markLayoutDirtySubtree();
        }

        void Tooltip::onRefresh()
        {
            setEscapesOverflow(isVisible);
        }

        bool Tooltip::isPinned() const
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

        Component* Tooltip::findAnchor() const
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

        bool Tooltip::isAnchorHovered(const Component* inAnchor) const
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
