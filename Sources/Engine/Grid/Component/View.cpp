#include "Chicane/Grid/Component/View.reflected.hpp"

#include <algorithm>
#include <stdexcept>

#include "Chicane/Core/Input/Keyboard/Event.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Input/Mouse/Wheel/Event.hpp"
#include "Chicane/Core/Input/Text/Event.hpp"

#include "Chicane/Grid/Component/Text/Glyph.hpp"
#include "Chicane/Grid/Component/View/InputQueue/Event.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View()
            : Component(TAG_ID),
              m_path(""),
              m_hovered(nullptr),
              m_focused(nullptr),
              m_dragging(nullptr),
              m_inputs(std::make_unique<ViewInputQueue>()),
              m_pointer(WindowCursor::Default)
        {
            m_root   = this;
            m_parent = this;
        }

        View::View(const FileSystem::Path& inTemplate, const FileSystem::Path& inStyle)
            : View()
        {
            load(inTemplate, inStyle);
        }

        View::~View() = default;

        void View::tick(float inDelta)
        {
            pump();

            Component::tick(inDelta);

            m_pointer.store(resolvePointer(), std::memory_order_relaxed);
        }

        void View::load(const FileSystem::Path& inTemplate, const FileSystem::Path& inStyle)
        {
            Component::load(inTemplate, inStyle);

            if (!inTemplate.isEmpty() && !String(m_sourceNode.name()).equals(TAG_ID))
            {
                throw std::runtime_error("UI document root element must be a " + String(TAG_ID));
            }

            m_path = getAttribute(PATH_ATTRIBUTE_NAME);
        }

        std::vector<Component*> View::getChildrenAt(const Vec2& inLocation) const
        {
            std::vector<Component*> contenders;
            for (Component* child : getChildrenFlat())
            {
                if (!child->isDrawable())
                {
                    continue;
                }

                if (!child->containsPoint(inLocation))
                {
                    continue;
                }

                contenders.push_back(child);
            }

            std::sort(
                contenders.begin(),
                contenders.end(),
                [](Component* inA, Component* inB) { return inA->getDepth() > inB->getDepth(); }
            );

            return contenders;
        }

        const StyleFile& View::getStyleFile() const
        {
            if (m_styles)
            {
                return *m_styles;
            }

            return StyleFile::empty();
        }

        Component* View::resolveHit(Component* inHit) const
        {
            Component* node = inHit;
            while (node && node->getTag().equals(TextGlyph::TAG_ID))
            {
                node = node->getParent();
            }

            if (node == this)
            {
                return nullptr;
            }

            return node;
        }

        Component* View::resolveFocus(Component* inHit) const
        {
            Component* node = inHit;
            while (node && node != this)
            {
                if (node->isFocusable())
                {
                    return node;
                }

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            return nullptr;
        }

        void View::handle(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::WindowMouseLeave)
            {
                syncHovered(nullptr);

                return;
            }

            if (inEvent.type == WindowEventType::WindowFocusLost || inEvent.type == WindowEventType::WindowFocusGained)
            {
                if (inEvent.type == WindowEventType::WindowFocusLost)
                {
                    syncDragging(nullptr);
                }

                if (m_focused)
                {
                    m_focused->onEvent(inEvent);
                }

                return;
            }

            if (inEvent.type == WindowEventType::MouseButtonUp)
            {
                Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
                if (event.button == Input::MouseButton::Left)
                {
                    syncDragging(nullptr);
                }
            }

            if (inEvent.type == WindowEventType::MouseButtonUp || inEvent.type == WindowEventType::MouseMotion)
            {
                if (broadcastEvent(inEvent))
                {
                    return;
                }
            }

            if (inEvent.type == WindowEventType::MouseButtonDown)
            {
                Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
                bubbleEvent(inEvent, event.location);

                Component* hit  = resolveHit(getHitAt(event.location));
                Component* node = hit;
                while (node && node != this)
                {
                    node->click();

                    if (node->isRoot())
                    {
                        break;
                    }

                    node = node->getParent();
                }

                syncFocused(resolveFocus(hit));

                if (event.button == Input::MouseButton::Left)
                {
                    syncDragging(hit);
                }
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent.data);
                syncHovered(resolveHit(getHitAt(event.location)));
            }

            if (inEvent.type == WindowEventType::MouseWheel)
            {
                Input::MouseWheelEvent event = *static_cast<Input::MouseWheelEvent*>(inEvent.data);
                bubbleEvent(inEvent, event.location);
            }

            if (inEvent.type == WindowEventType::KeyDown || inEvent.type == WindowEventType::KeyUp ||
                inEvent.type == WindowEventType::TextInput)
            {
                Component* node = m_focused;
                while (node && node != this)
                {
                    if (node->onEvent(inEvent))
                    {
                        return;
                    }

                    if (node->isRoot())
                    {
                        break;
                    }

                    node = node->getParent();
                }
            }
        }

        WindowCursor View::getPointer() const
        {
            return m_pointer.load(std::memory_order_relaxed);
        }

        WindowCursor View::resolvePointer() const
        {
            Component* node = m_hovered;
            while (node)
            {
                if (!node->getStyle().cursor.getRaw().isEmpty())
                {
                    return node->getStyle().cursor.get();
                }

                if (node->isRoot() || !node->hasParent())
                {
                    break;
                }

                node = node->getParent();
            }

            return WindowCursor::Default;
        }

        void View::post(const WindowEvent& inEvent)
        {
            switch (inEvent.type)
            {
            case WindowEventType::WindowMouseLeave:
            case WindowEventType::WindowFocusLost:
            case WindowEventType::WindowFocusGained:
            case WindowEventType::MouseMotion:
            case WindowEventType::MouseButtonDown:
            case WindowEventType::MouseButtonUp:
            case WindowEventType::MouseWheel:
            case WindowEventType::KeyDown:
            case WindowEventType::KeyUp:
            case WindowEventType::TextInput:
                break;

            default:
                return;
            }

            ViewInputQueue&   queue = *m_inputs;
            const std::size_t write = queue.write.load(std::memory_order_relaxed);
            const std::size_t next  = (write + 1) % ViewInputQueue::CAPACITY;

            if (next == queue.read.load(std::memory_order_acquire))
            {
                return;
            }

            ViewInputQueueEvent& slot = queue.events[write];
            slot.type                 = inEvent.type;

            if (inEvent.data)
            {
                switch (inEvent.type)
                {
                case WindowEventType::MouseMotion:
                    slot.motion = *static_cast<const Input::MouseMotionEvent*>(inEvent.data);

                    break;

                case WindowEventType::MouseButtonDown:
                case WindowEventType::MouseButtonUp:
                    slot.button = *static_cast<const Input::MouseButtonEvent*>(inEvent.data);

                    break;

                case WindowEventType::MouseWheel:
                    slot.wheel = *static_cast<const Input::MouseWheelEvent*>(inEvent.data);

                    break;

                case WindowEventType::KeyDown:
                case WindowEventType::KeyUp:
                    slot.keyboard = *static_cast<const Input::KeyboardEvent*>(inEvent.data);

                    break;

                case WindowEventType::TextInput:
                    slot.text = *static_cast<const Input::TextEvent*>(inEvent.data);

                    break;

                default:
                    break;
                }
            }

            queue.write.store(next, std::memory_order_release);
        }

        void View::pump()
        {
            ViewInputQueue&   queue = *m_inputs;
            std::size_t       read  = queue.read.load(std::memory_order_relaxed);
            const std::size_t write = queue.write.load(std::memory_order_acquire);

            while (read != write)
            {
                ViewInputQueueEvent& slot = queue.events[read];

                WindowEvent event;
                event.type = slot.type;

                switch (slot.type)
                {
                case WindowEventType::MouseMotion:
                    event.data = &slot.motion;

                    break;

                case WindowEventType::MouseButtonDown:
                case WindowEventType::MouseButtonUp:
                    event.data = &slot.button;

                    break;

                case WindowEventType::MouseWheel:
                    event.data = &slot.wheel;

                    break;

                case WindowEventType::KeyDown:
                case WindowEventType::KeyUp:
                    event.data = &slot.keyboard;

                    break;

                case WindowEventType::TextInput:
                    event.data = &slot.text;

                    break;

                default:
                    event.data = nullptr;

                    break;
                }

                handle(event);

                read = (read + 1) % ViewInputQueue::CAPACITY;
            }

            queue.read.store(read, std::memory_order_release);
        }

        void View::clearInteraction(Component* inComponent)
        {
            if (m_hovered == inComponent)
            {
                m_hovered = nullptr;
            }

            if (m_focused == inComponent)
            {
                m_focused = nullptr;
            }

            if (m_dragging == inComponent)
            {
                syncDragging(nullptr);
            }
        }

        void View::focusOn(Component* inComponent)
        {
            syncFocused(inComponent);
        }

        void View::syncHovered(Component* inComponent)
        {
            Component* hit = (inComponent == this) ? nullptr : inComponent;

            std::vector<Component*> next;
            Component*              node = hit;
            while (node && node != this)
            {
                next.push_back(node);

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            std::vector<Component*> previous;
            node = m_hovered;
            while (node && node != this)
            {
                previous.push_back(node);

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            for (Component* candidate : previous)
            {
                bool bKeep = false;
                for (Component* kept : next)
                {
                    if (kept == candidate)
                    {
                        bKeep = true;

                        break;
                    }
                }

                if (!bKeep)
                {
                    candidate->setHovered(false);
                }
            }

            for (auto it = next.rbegin(); it != next.rend(); ++it)
            {
                (*it)->setHovered(true);
            }

            m_hovered = hit;
        }

        void View::syncFocused(Component* inComponent)
        {
            Component* next = (inComponent == this) ? nullptr : inComponent;

            if (m_focused == next)
            {
                return;
            }

            if (m_focused)
            {
                m_focused->setFocused(false);
            }

            m_focused = next;

            if (m_focused)
            {
                m_focused->setFocused(true);
            }
        }

        void View::syncDragging(Component* inComponent)
        {
            Component* hit = (inComponent == this) ? nullptr : inComponent;

            if (m_dragging == hit)
            {
                return;
            }

            std::vector<Component*> next;
            Component*              node = hit;
            while (node && node != this)
            {
                next.push_back(node);

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            std::vector<Component*> previous;
            node = m_dragging;
            while (node && node != this)
            {
                previous.push_back(node);

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            for (Component* candidate : previous)
            {
                bool bKeep = false;
                for (Component* kept : next)
                {
                    if (kept == candidate)
                    {
                        bKeep = true;

                        break;
                    }
                }

                if (!bKeep)
                {
                    candidate->setDragging(false);
                }
            }

            for (auto it = next.rbegin(); it != next.rend(); ++it)
            {
                (*it)->setDragging(true);
            }

            m_dragging = hit;
        }
    }
}
