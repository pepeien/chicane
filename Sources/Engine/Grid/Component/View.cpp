#include "Chicane/Grid/Component/View.reflected.hpp"

#include <algorithm>
#include <stdexcept>

#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Input/Mouse/Wheel/Event.hpp"

#include "Chicane/Grid/Component/Text/Glyph.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View()
            : Component(TAG_ID),
              m_path(""),
              m_hovered(nullptr),
              m_focused(nullptr)
        {
            m_root   = this;
            m_parent = this;
        }

        View::View(const FileSystem::Path& inTemplate, const FileSystem::Path& inStyle)
            : View()
        {
            load(inTemplate, inStyle);
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
                [](Component* inA, Component* inB)
                { return inA->getStyle().zIndex.get() > inB->getStyle().zIndex.get(); }
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
                if (m_focused)
                {
                    m_focused->onEvent(inEvent);
                }

                return;
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
    }
}
