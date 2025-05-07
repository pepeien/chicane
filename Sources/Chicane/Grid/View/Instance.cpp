#include "Grid/View/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View(const std::string& inId, const std::string& inSource)
            : Component(TAG_ID, inId),
            m_styles({})
        {
            if (inSource.empty())
            {
                return;
            }

            if (!m_document.load_file(inSource.c_str()))
            {
                throw std::runtime_error("Failed to read " + inSource);
            }

            if (m_document.empty() || m_document.children().empty())
            {
                throw std::runtime_error("UI document " + inSource + " does not have any components");
            }

            const pugi::xml_node& node = m_document.first_child();

            bool bIsRoot  = node.parent() == node.root() && !node.next_sibling();

            if (!bIsRoot)
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            if (!String::areEquals(TAG_ID, node.name()))
            {
                throw std::runtime_error("UI document root element must be a " + std::string(TAG_ID));
            }

            m_styles = Style::Instance::parseSources(node);
            m_style.position = Style::POSITION_TYPE_ABSOLUTE;

            m_root = this;
            m_parent = this;

            addChildren(node);
        }

        void View::onChildAddition(Component* inComponent)
        {
            inComponent->setStyle(m_styles);
        }

        void View::onEvent(const SDL_Event& inEvent)
        {
        }

        Window::Instance* View::getWindow() const
        {
            return m_window;
        }

        void View::setWindow(Window::Instance* inWindow)
        {
            m_window = inWindow;

            if (m_window == nullptr)
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
    }
}