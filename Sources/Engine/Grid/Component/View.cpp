#include "Chicane/Grid/Component/View.hpp"

#include <algorithm>

#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View(const String& inSource)
            : Component(TAG_ID),
              m_styles({})
        {
            if (inSource.isEmpty())
            {
                return;
            }

            pugi::xml_document document;

            if (!document.load_file(inSource.toChar()))
            {
                throw std::runtime_error("Failed to read " + inSource);
            }

            if (document.empty() || document.children().empty())
            {
                throw std::runtime_error("UI document " + inSource + " does not have any components");
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

            m_styles = Style::parseSources(node);

            m_root   = this;
            m_parent = this;

            addChildren(node);
        }

        void View::onAdopted(Component* inComponent)
        {
            inComponent->setStyle(m_styles);
        }

        void View::activate()
        {
            onActivation();
        }

        void View::deactivate()
        {
            onDeactivation();
        }

        void View::handle(WindowEvent inEvent)
        {
            if (inEvent.type == WindowEventType::MouseButtonDown)
            {
                Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);

                for (Component* contender : getChildrenOn(event.location))
                {
                    contender->click();
                }
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent.data);

                for (Component* contender : getChildrenOn(event.location))
                {
                    contender->hover();
                }
            }
        }

        std::vector<Component*> View::getFlatChildren(const Component* inParent) const
        {
            const Component* parent = inParent;

            if (!parent)
            {
                parent = this;
            }

            std::vector<Component*> result;

            for (Component* child : parent->getChildren())
            {
                result.push_back(child);

                std::vector<Component*> subChildren = getFlatChildren(child);

                result.insert(result.end(), subChildren.begin(), subChildren.end());
            }

            return result;
        }

        std::vector<Component*> View::getChildrenOn(const Vec2& inLocation) const
        {
            std::vector<Component*> contenders = {};
            for (Component* child : getFlatChildren())
            {
                if (!child->isDrawable())
                {
                    continue;
                }

                if (!child->getBounds().contains(inLocation))
                {
                    continue;
                }

                contenders.push_back(child);
            }

            std::sort(
                contenders.begin(),
                contenders.end(),
                [](Component* inA, Component* inB) { return inA->getStyle().zIndex > inB->getStyle().zIndex; }
            );

            return contenders;
        }
    }
}