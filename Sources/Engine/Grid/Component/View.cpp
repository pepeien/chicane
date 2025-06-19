#include "Chicane/Grid/Component/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View(const String& inSource)
            : Super(TAG_ID),
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

        const String& View::getPath() const
        {
            return m_path;
        }
    }
}