#include "Chicane/Grid/Component/Window.reflected.hpp"

#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Window/Event/Type.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid/Component/Button.hpp"

namespace Chicane
{
    namespace Grid
    {
        Window::Window(const XmlNode& inNode)
            : Container(inNode),
              bIsVisible(true),
              hasTitle(false),
              title(String::sEmpty()),
              m_handleId(String::sEmpty()),
              m_bIsGrabbable(true),
              m_bIsMoving(false),
              m_move(Vec2::sZero()),
              m_moveCursor(Vec2::sZero())
        {
            load("Assets/Engine/UI/Components/Window/Index.grid", "Assets/Engine/UI/Components/Window/Index.decal");

            watchAttribute(
                IS_OPEN_ATTRIBUTE_NAME,
                [this](const String& inValue)
                {
                    if (inValue.isEmpty())
                    {
                        bIsVisible = true;
                    }
                    else
                    {
                        bIsVisible = Xml::parseBool(parseText(inValue).trim(), true);
                    }
                }
            );

            watchAttribute(
                TITLE_ATTRIBUTE_NAME,
                [this](const String& inValue)
                {
                    title = parseText(inValue).trim();
                    refreshTitleVisibility();
                }
            );

            watchAttribute(
                HANDLE_ATTRIBUTE_NAME,
                [this](const String& inValue)
                {
                    m_handleId = parseText(inValue).trim();
                    refreshTitleVisibility();
                }
            );

            watchAttribute(
                IS_GRABBABLE_ATTRIBUTE_NAME,
                [this](const String& inValue) { m_bIsGrabbable = Xml::parseBool(parseText(inValue).trim(), true); }
            );
        }

        Window* Window::sFindFrom(Component* inComponent)
        {
            Component* node = inComponent;
            while (node)
            {
                if (node->getTag().equals(TAG_ID))
                {
                    return static_cast<Window*>(node);
                }

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            return nullptr;
        }

        bool Window::isFocusable() const
        {
            return false;
        }

        bool Window::escapesOverflow() const
        {
            return true;
        }

        bool Window::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::MouseButtonUp)
            {
                if (!m_bIsMoving)
                {
                    return false;
                }

                endMove();

                return true;
            }

            if (inEvent.type == WindowEventType::MouseButtonDown)
            {
                if (!inEvent.data)
                {
                    return false;
                }

                const Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
                if (event.button != Input::MouseButton::Left || !containsPoint(event.location))
                {
                    return false;
                }

                Component* hit = hasRoot() ? getRoot()->getHitAt(event.location) : nullptr;
                if (!canMoveFrom(hit))
                {
                    return false;
                }

                beginMove(event.location);

                return true;
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                if (!m_bIsMoving || !inEvent.data)
                {
                    return false;
                }

                const Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent.data);
                updateMove(event.location);

                return true;
            }

            return false;
        }

        void Window::tick(float inDeltaTime)
        {
            refreshOpenState();
            Container::tick(inDeltaTime);
        }

        void Window::refreshPosition()
        {
            Container::refreshPosition();
            addPosition(m_move);
        }

        void Window::dismiss()
        {
            endMove();
            bIsVisible = false;
            getMethod(getAttribute(ON_CLOSE_ATTRIBUTE_NAME)).invoke();
        }

        bool Window::isGrabbable() const
        {
            return m_bIsGrabbable;
        }

        void Window::setGrabbable(bool inValue)
        {
            m_bIsGrabbable = inValue;
            setAttribute(IS_GRABBABLE_ATTRIBUTE_NAME, inValue ? "true" : "false");
        }

        bool Window::hasAssignedHandle() const
        {
            return findAssignedHandle() != nullptr;
        }

        bool Window::isAssignedHandle(const Component* inComponent) const
        {
            if (!inComponent || m_handleId.isEmpty())
            {
                return false;
            }

            return inComponent->getId().equals(m_handleId);
        }

        Component* Window::findAssignedHandle() const
        {
            if (m_handleId.isEmpty())
            {
                return nullptr;
            }

            if (getId().equals(m_handleId))
            {
                return const_cast<Window*>(this);
            }

            for (Component* child : Component::getChildrenFlat())
            {
                if (child && child->getId().equals(m_handleId))
                {
                    return child;
                }
            }

            return nullptr;
        }

        void Window::refreshOpenState()
        {
            const String isOpen = getAttribute(IS_OPEN_ATTRIBUTE_NAME);
            if (!isOpen.isEmpty())
            {
                bIsVisible = Xml::parseBool(parseText(isOpen).trim(), true);

                return;
            }

            const String condition = getAttribute(IF_DIRECTIVE_KEYWORD);
            if (!condition.isEmpty())
            {
                bIsVisible = parseText(condition).equals("true", "1");
            }
        }

        void Window::refreshTitleVisibility()
        {
            hasTitle = !title.isEmpty() && !hasAssignedHandle();
        }

        bool Window::canMoveFrom(Component* inHit) const
        {
            if (!inHit || !isGrabbable())
            {
                return false;
            }

            if (inHit->getTag().equals(Button::TAG_ID) && !isAssignedHandle(inHit))
            {
                return false;
            }

            for (Component* node = inHit; node != nullptr; node = node->getParent())
            {
                if (isAssignedHandle(node))
                {
                    return true;
                }

                if (!hasAssignedHandle() && node->getClassName().contains(BAR_CLASS_NAME))
                {
                    return true;
                }

                if (node == this)
                {
                    return !hasAssignedHandle() && !hasTitle;
                }

                if (node->isRoot())
                {
                    break;
                }
            }

            return false;
        }

        void Window::beginMove(const Vec2& inLocation)
        {
            m_bIsMoving  = true;
            m_moveCursor = inLocation;
            setDragging(true);
        }

        void Window::updateMove(const Vec2& inLocation)
        {
            m_move.x += inLocation.x - m_moveCursor.x;
            m_move.y += inLocation.y - m_moveCursor.y;
            m_moveCursor = inLocation;
            markLayoutDirtySubtree();
        }

        void Window::endMove()
        {
            if (!m_bIsMoving)
            {
                return;
            }

            m_bIsMoving = false;
            setDragging(false);
        }
    }
}
