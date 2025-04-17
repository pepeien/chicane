#include "Grid/View.hpp"

#include "Core.hpp"
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
            const Component::Variables& inVariables,
            const Component::Functions& inFunctions
        )
            : m_id(inId),
            m_variables(inVariables),
            m_functions(inFunctions),
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

            assertProps(m_document.first_child());

            addView(this);
        }

        View::View(const std::string& inId)
            : m_id(inId),
            m_variables({}),
            m_functions({}),
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

        const std::string& View::getId() const
        {
            return m_id;
        }

        const Vec<2, int>& View::getSize() const
        {
            return m_size;
        }

        void View::setSize(const Vec<2, int>& inSize)
        {
            setSize(inSize.x, inSize.y);
        }

        void View::setSize(int inWidth, int inHeight)
        {
            if (m_size.x == inWidth && m_size.y == inWidth)
            {
                return;
            }

            m_size.x = inWidth;
            m_size.y = inHeight;
        }

        const Vec<2, int>& View::getPosition() const
        {
            return m_position;
        }

        void View::setPosition(const Vec<2, int>& inPosition)
        {
            setPosition(inPosition.x, inPosition.y);
        }

        void View::setPosition(int inX, int inY)
        {
            if (m_position.x == inX && m_position.y == inX)
            {
                return;
            }

            m_position.x = inX;
            m_position.y = inY;
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

        bool View::hasVariable(const std::string& inId) const
        {
            return m_variables.find(inId) != m_variables.end() &&
                   m_variables.at(inId) && !m_variables.at(inId)->isEmpty();
        }

        Reference* View::getVariable(const std::string& inId) const
        {
            if (!hasVariable(inId))
            {
                return nullptr;
            }

            return m_variables.at(inId);
        }

        void View::addVariable(const Component::Variables& inVariables)
        {
            for (auto [id, variable] : inVariables)
            {
                addVariable(id, variable);
            }
        }

        void View::addVariable(const std::string& inId, Reference* inVariable)
        {
            if (hasVariable(inId))
            {
                return;
            }

            m_variables.insert(
                std::make_pair(
                    inId,
                    inVariable
                )
            );
        }

        void View::removeVariable(const std::string& inId)
        {
            if (!hasVariable(inId))
            {
                return;
            }

            m_variables.erase(inId);
        }

        bool View::hasFunction(const std::string& inId) const
        {
            return m_functions.find(inId) != m_functions.end();
        }

        const Component::Function View::getFunction(const std::string& inId) const
        {
            if (!hasFunction(inId))
            {
                return nullptr;
            }

            return m_functions.at(inId);
        }

        void View::addFunction(const Component::Functions& inFunctions)
        {
            for (auto [id, function] : inFunctions)
            {
                addFunction(id, function);
            }
        }

        void View::addFunction(const std::string& inId, Component::Function inFunction)
        {
            if (hasFunction(inId))
            {
                return;
            }

            m_functions.insert(
                std::make_pair(
                    inId,
                    inFunction
                )
            );
        }

        void View::removeFunction(const std::string& inId)
        {
            if (!hasFunction(inId))
            {
                return;
            }

            m_functions.erase(inId);
        }

        void View::assertProps(const pugi::xml_node& inNode)
        {
            bool bIsRoot  = inNode.parent() == inNode.root();
            bool bIsAlone = bIsRoot && !inNode.next_sibling();

            if (!bIsRoot || !bIsAlone)
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            if (!String::areEquals(TAG_ID, inNode.name()))
            {
                throw std::runtime_error("UI document root element must be a " + TAG_ID);
            }
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