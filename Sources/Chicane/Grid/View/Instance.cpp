#include "Grid/View/Instance.hpp"

#include "Grid/Essential.hpp"
#include "Grid/Style.hpp"

static const std::string TAG_ID = "View";

namespace Chicane
{
    namespace Grid
    {
        View::View(
            const std::string& inId,
            const std::string& inSource,
            const References& inReferences,
            const Functions& inFunctions
        )
            : Component(inId),
            m_bWasStyleAdded(false)
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

            bool bIsRoot  = node.parent() == node.root();
            bool bIsAlone = bIsRoot && !node.next_sibling();

            if (!bIsRoot || !bIsAlone)
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            if (!String::areEquals(TAG_ID, node.name()))
            {
                throw std::runtime_error("UI document root element must be a " + TAG_ID);
            }

            addView(this);
        }

        View::View(const std::string& inId)
            : Component(inId),
            m_bWasStyleAdded(false)
        {}

        void View::rebuild()
        {
            m_bWasStyleAdded = false;
        }

        void View::activate()
        {
            setView(m_id);
        }

        void View::show(
            const Vec<2, int>& inSize,
            const Vec<2, int>& inPosition
        )
        {
            compile(
                m_document.first_child(),
                inSize,
                inPosition
            );
        }

        void View::compile(
            const pugi::xml_node& inNode,
            const Vec<2, int>& inSize,
            const Vec<2, int>& inPosition
        )
        {
    /*
            m_size.x = static_cast<float>(inSize.x);
            m_size.y = static_cast<float>(inSize.y);
            ImGui::SetNextWindowContentSize(m_size);
            ImGui::SetNextWindowSize(       m_size);

            m_position.x = static_cast<float>(inPosition.x);
            m_position.y = static_cast<float>(inPosition.y);
            ImGui::SetNextWindowPos(m_position);

            if (!m_bWasStyleAdded)
            {
                Style::addStyle(inNode);

                m_bWasStyleAdded = true;
            }

            execOnTick(inNode);

            ImGui::Begin(
                m_id.c_str(),
                nullptr,
                m_flags
            );
                compileChildren(inNode);
            ImGui::End();
*/
        }
    }
}