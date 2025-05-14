#include "Chicane/Grid/View/Instance.hpp"

#include "Chicane/Grid/Loader.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View(const std::string& inSource)
            : Component(TAG_ID),
            m_styles({})
        {
            if (inSource.empty())
            {
                return;
            }

            pugi::xml_document document;

            if (!document.load_file(inSource.c_str()))
            {
                throw std::runtime_error("Failed to read " + inSource);
            }

            if (document.empty() || document.children().empty())
            {
                throw std::runtime_error("UI document " + inSource + " does not have any components");
            }

            const pugi::xml_node& node = document.first_child();

            bool bIsRoot = node.parent() == node.root() && !node.next_sibling();

            if (!bIsRoot)
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            if (!String::areEquals(TAG_ID, node.name()))
            {
                throw std::runtime_error("UI document root element must be a " + std::string(TAG_ID));
            }

            m_path = XML::getAttribute(PATH_ATTRIBUTE_NAME, node).as_string();

            m_styles = Style::Instance::parseSources(node);

            m_root   = this;
            m_parent = this;

            addChildren(node);

            registerView(this);
        }

        void View::onChildAddition(Component* inComponent)
        {
            inComponent->setStyle(m_styles);
        }

        void View::activate()
        {
            activateView(m_path);

            onActivation();
        }

        const std::string& View::getPath() const
        {
            return m_path;
        }

        Window::Instance* View::getWindow() const
        {
            return m_window;
        }

        void View::setWindow(Window::Instance* inWindow)
        {
            m_window = inWindow;

            if (!m_window)
            {
                return;
            }

            m_window->watchSize(
                [this](const Vec<2, int>& inSize)
                {
                    setSize(inSize.x, inSize.y);
                }
            );
        }

        std::vector<const Component*> View::getDrawableChildren() const
        {
            std::vector<const Component*> result = {};

            for (const Component* child : flatChildren())
            {
                if (!child->isDrawable())
                {
                    continue;
                }

                result.push_back(child);
            }

            return result;
        }

        std::vector<const Component*> View::flatChildren() const
        {
            return flatChildren(this);
        }

        std::vector<const Component*> View::flatChildren(const Component* inParent) const
        {
            if (!inParent)
            {
                return {};
            }

            std::vector<const Component*> result = {};

            for (const Component* child : inParent->getChildren())
            {
                if (!child)
                {
                    continue;
                }

                result.push_back(child);

                for (const Component* grandChild : flatChildren(child))
                {
                    result.push_back(grandChild);
                }
            }

            return result;
        }
    }
}