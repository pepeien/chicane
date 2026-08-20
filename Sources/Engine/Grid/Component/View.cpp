#include "Chicane/Grid/Component/View.reflected.hpp"

#include <algorithm>

#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Input/Mouse/Wheel/Event.hpp"

#include "Chicane/Grid/Component/Text/Glyph.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View(const FileSystem::Path& inSource)
            : Component(TAG_ID),
              m_path(""),
              m_styles({}),
              m_hovered(nullptr),
              m_focused(nullptr)
        {
            if (inSource.isEmpty())
            {
                return;
            }

            pugi::xml_document document;

            if (!document.load_file(inSource.toChar()))
            {
                throw std::runtime_error("Failed to read " + inSource.toString());
            }

            if (document.empty() || document.children().empty())
            {
                throw std::runtime_error("UI document " + inSource.toString() + " does not have any components");
            }

            const pugi::xml_node& node = document.first_child();

            const bool bIsRoot = node.parent() == node.root() && !node.next_sibling();

            if (!bIsRoot)
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            const String name = node.name();

            if (!name.equals(TAG_ID))
            {
                throw std::runtime_error("UI document root element must be a " + String(TAG_ID));
            }

            m_path = Xml::getAttribute(PATH_ATTRIBUTE_NAME, node).as_string();

            m_sourceNode = m_sourceDocument.append_copy(node);
            m_attributes = Xml::getAttributes(m_sourceNode);

            m_root   = this;
            m_parent = this;

            importStyleFile(Xml::getAttribute(Style::ATTRIBUTE_NAME, node).as_string());
            setId(getAttribute(ID_ATTRIBUTE_NAME));
            setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));

            addChildren(node);
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
            return m_styles;
        }

        void View::importStyleFile(const FileSystem::Path& inValue)
        {
            m_styles.parse(inValue);

            setStyleFile(&m_styles);
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

        void View::handle(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::WindowMouseLeave ||
                inEvent.type == WindowEventType::WindowFocusLost)
            {
                if (inEvent.type == WindowEventType::WindowMouseLeave)
                {
                    syncHovered(nullptr);
                }

                if (inEvent.type == WindowEventType::WindowFocusLost)
                {
                    syncFocused(nullptr);
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
                if (bubbleEvent(inEvent, event.location))
                {
                    return;
                }

                Component* hit = resolveHit(getHitAt(event.location));
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

                syncFocused(hit);
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

            if (
                inEvent.type == WindowEventType::KeyDown ||
                inEvent.type == WindowEventType::KeyUp ||
                inEvent.type == WindowEventType::TextInput
            )
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
