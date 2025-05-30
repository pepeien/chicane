#include "Chicane/Grid/Component/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View(const std::string& inSource)
            : Super(TAG_ID),
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

            m_path = Xml::getAttribute(PATH_ATTRIBUTE_NAME, node).as_string();

            m_styles = Style::Instance::parseSources(node);

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

        const std::string& View::getPath() const
        {
            return m_path;
        }
    }
}