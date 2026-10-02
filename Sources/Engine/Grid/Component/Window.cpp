#include "Chicane/Grid/Component/Window.reflected.hpp"

#include <algorithm>
#include <cstdint>

#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/Window/Cursor.hpp"
#include "Chicane/Core/Window/Event/Type.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid/Component/Button.hpp"
#include "Chicane/Grid/Style.hpp"
#include "Chicane/Grid/Style/Display.hpp"

namespace Chicane
{
    namespace Grid
    {
        constexpr std::uint8_t WINDOW_RESIZE_LEFT   = 1 << 0;
        constexpr std::uint8_t WINDOW_RESIZE_RIGHT  = 1 << 1;
        constexpr std::uint8_t WINDOW_RESIZE_TOP    = 1 << 2;
        constexpr std::uint8_t WINDOW_RESIZE_BOTTOM = 1 << 3;

        Window::Window(const XmlNode& inNode)
            : Container(inNode),
              bIsVisible(true),
              hasTitle(false),
              title(String::sEmpty()),
              m_handleId(String::sEmpty()),
              m_bIsGrabbable(true),
              m_bIsResizable(false),
              m_bIsMoving(false),
              m_bHasExtent(false),
              m_move(Vec2::sZero()),
              m_moveCursor(Vec2::sZero()),
              m_extent(Vec2::sZero()),
              m_resizeEdge(0),
              m_resizeCursor(Vec2::sZero())
        {
            load("Assets/Engine/UI/Components/Window/Index.grid", "Assets/Engine/UI/Components/Window/Index.decal");

            watchAttribute(
                IS_OPEN_ATTRIBUTE_NAME,
                [this](const String& inValue)
                {
                    const bool bValueEmpty = static_cast<bool>(inValue.isEmpty());

                    if (bValueEmpty)
                    {
                        bIsVisible = true;
                    }

                    if (!bValueEmpty)
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

            watchAttribute(
                IS_RESIZABLE_ATTRIBUTE_NAME,
                [this](const String& inValue) { m_bIsResizable = Xml::parseBool(parseText(inValue).trim(), false); }
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

        bool Window::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::MouseButtonUp)
            {
                if (m_resizeEdge != 0)
                {
                    endResize();

                    return true;
                }

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
                if (hit && hit->getTag().equals(Button::TAG_ID) && !isAssignedHandle(hit))
                {
                    return false;
                }

                const std::uint8_t edge = hitResize(event.location);
                if (edge != 0)
                {
                    beginResize(edge, event.location);

                    return true;
                }

                if (!canMoveFrom(hit))
                {
                    return false;
                }

                beginMove(event.location);

                return true;
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                if (!inEvent.data)
                {
                    return false;
                }

                const Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent.data);

                if (m_resizeEdge != 0)
                {
                    updateResize(event.location);

                    return true;
                }

                if (m_bIsMoving)
                {
                    updateMove(event.location);

                    return true;
                }

                refreshResizeCursor(event.location);

                return false;
            }

            return false;
        }

        void Window::tick(float inDeltaTime)
        {
            refreshOpenState();

            if (bIsVisible && style.isDisplay(StyleDisplay::None))
            {
                const bool bDisplayEmpty = static_cast<bool>(style.display.getRaw().isEmpty());

                if (bDisplayEmpty)
                {
                    style.display.set(StyleDisplay::Block);
                }

                if (!bDisplayEmpty)
                {
                    style.display.refresh();
                }

                if (style.isDisplay(StyleDisplay::None))
                {
                    style.display.set(StyleDisplay::Block);
                }

                markLayoutDirtySubtree();
            }

            Container::tick(inDeltaTime);

            if (!bIsVisible)
            {
                style.display.set(StyleDisplay::None);
            }
        }

        void Window::refreshSize()
        {
            if (m_bHasExtent)
            {
                float width  = m_extent.x;
                float height = m_extent.y;
                style.width.clamp(width);
                style.height.clamp(height);
                m_extent.x = width;
                m_extent.y = height;
                setSize(width, height);
                setFlag(ComponentDirty::Insets);

                return;
            }

            Container::refreshSize();
        }

        void Window::refreshPosition()
        {
            Container::refreshPosition();
            addPosition(m_move);
        }

        void Window::dismiss()
        {
            endResize();
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

        bool Window::isResizable() const
        {
            return m_bIsResizable;
        }

        void Window::setResizable(bool inValue)
        {
            m_bIsResizable = inValue;
            setAttribute(IS_RESIZABLE_ATTRIBUTE_NAME, inValue ? "true" : "false");
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
                bIsVisible = Xml::parseBool(parseText(condition).trim(), true);

                return;
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
            setDragging(true, false);
        }

        void Window::updateMove(const Vec2& inLocation)
        {
            const Vec2 delta(inLocation.x - m_moveCursor.x, inLocation.y - m_moveCursor.y);
            m_move.x += delta.x;
            m_move.y += delta.y;
            m_moveCursor = inLocation;
            shift(delta);
        }

        void Window::shift(const Vec2& inDelta)
        {
            if (inDelta.x == 0.0f && inDelta.y == 0.0f)
            {
                return;
            }

            addPosition(inDelta);

            for (Component* child : getChildrenFlat())
            {
                if (child)
                {
                    child->addPosition(inDelta);
                }
            }

            markPaintDirtySubtree();
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

        std::uint8_t Window::hitResize(const Vec2& inLocation) const
        {
            if (!isResizable() || !containsPoint(inLocation))
            {
                return 0;
            }

            const Bounds2D box  = getDrawBounds();
            const float    grip = resizeGrip();
            std::uint8_t   edge = 0;

            const bool bNearLeft = static_cast<bool>(inLocation.x <= box.left + grip);

            if (bNearLeft)
            {
                edge |= WINDOW_RESIZE_LEFT;
            }

            const bool bNearRight = !bNearLeft && (inLocation.x >= box.right - grip);

            if (bNearRight)
            {
                edge |= WINDOW_RESIZE_RIGHT;
            }

            const bool bNearTop = static_cast<bool>(inLocation.y <= box.top + grip);

            if (bNearTop)
            {
                edge |= WINDOW_RESIZE_TOP;
            }

            const bool bNearBottom = !bNearTop && (inLocation.y >= box.bottom - grip);

            if (bNearBottom)
            {
                edge |= WINDOW_RESIZE_BOTTOM;
            }

            return edge;
        }

        void Window::beginResize(std::uint8_t inEdge, const Vec2& inLocation)
        {
            m_resizeEdge   = inEdge;
            m_resizeCursor = inLocation;
            m_extent       = getSize();
            m_bHasExtent   = true;
            setDragging(true, false);
            applyResizeCursor(inEdge);
        }

        void Window::updateResize(const Vec2& inLocation)
        {
            const Vec2 delta = Vec2(inLocation.x - m_resizeCursor.x, inLocation.y - m_resizeCursor.y);
            m_resizeCursor   = inLocation;

            float width  = m_size.x;
            float height = m_size.y;

            const bool bLeft = static_cast<bool>(m_resizeEdge & WINDOW_RESIZE_LEFT);

            if (bLeft)
            {
                const float previous = width;
                width -= delta.x;
                style.width.clamp(width);
                m_move.x += previous - width;
            }

            const bool bRight = !bLeft && (m_resizeEdge & WINDOW_RESIZE_RIGHT);

            if (bRight)
            {
                width += delta.x;
                style.width.clamp(width);
            }

            const bool bTop = static_cast<bool>(m_resizeEdge & WINDOW_RESIZE_TOP);

            if (bTop)
            {
                const float previous = height;
                height -= delta.y;
                style.height.clamp(height);
                m_move.y += previous - height;
            }

            const bool bBottom = !bTop && (m_resizeEdge & WINDOW_RESIZE_BOTTOM);

            if (bBottom)
            {
                height += delta.y;
                style.height.clamp(height);
            }

            applyExtent(width, height);
        }

        void Window::endResize()
        {
            if (m_resizeEdge == 0)
            {
                return;
            }

            m_resizeEdge = 0;
            setDragging(false);
            clearCursor();
        }

        void Window::applyExtent(float inWidth, float inHeight)
        {
            float width  = std::max(0.0f, inWidth);
            float height = std::max(0.0f, inHeight);
            style.width.clamp(width);
            style.height.clamp(height);

            m_extent.x   = width;
            m_extent.y   = height;
            m_bHasExtent = true;

            style.width.value.setRaw(String::sSprint("%.0fpx", width));
            style.width.value.set(width);
            style.height.value.setRaw(String::sSprint("%.0fpx", height));
            style.height.value.set(height);

            setSize(width, height);
            markLayoutDirtySubtree();
        }

        void Window::applyResizeCursor(std::uint8_t inEdge)
        {
            const bool bLeft   = (inEdge & WINDOW_RESIZE_LEFT) != 0;
            const bool bRight  = (inEdge & WINDOW_RESIZE_RIGHT) != 0;
            const bool bTop    = (inEdge & WINDOW_RESIZE_TOP) != 0;
            const bool bBottom = (inEdge & WINDOW_RESIZE_BOTTOM) != 0;

            if ((bLeft && bTop) || (bRight && bBottom))
            {
                style.cursor.setRaw(Style::CURSOR_TYPE_NWSE_RESIZE);
                style.cursor.set(WindowCursor::NwseResize);

                return;
            }

            if ((bRight && bTop) || (bLeft && bBottom))
            {
                style.cursor.setRaw(Style::CURSOR_TYPE_NESW_RESIZE);
                style.cursor.set(WindowCursor::NeswResize);

                return;
            }

            if (bLeft || bRight)
            {
                style.cursor.setRaw(Style::CURSOR_TYPE_EW_RESIZE);
                style.cursor.set(WindowCursor::EwResize);

                return;
            }

            style.cursor.setRaw(Style::CURSOR_TYPE_NS_RESIZE);
            style.cursor.set(WindowCursor::NsResize);
        }

        void Window::refreshResizeCursor(const Vec2& inLocation)
        {
            const std::uint8_t edge = hitResize(inLocation);
            if (edge != 0)
            {
                applyResizeCursor(edge);

                return;
            }

            if (style.cursor.get() == WindowCursor::NsResize || style.cursor.get() == WindowCursor::EwResize ||
                style.cursor.get() == WindowCursor::NeswResize || style.cursor.get() == WindowCursor::NwseResize)
            {
                clearCursor();
            }
        }

        void Window::clearCursor()
        {
            style.cursor.setRaw("");
            style.cursor.set(WindowCursor::Default);
        }

        float Window::resizeGrip() const
        {
            return std::max(6.0f, style.font.size.get() * 0.45f);
        }
    }
}
