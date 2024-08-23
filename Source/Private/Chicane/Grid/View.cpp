#include "Chicane/Grid/View.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        View::View(
            const std::string& inId,
            const std::string& inSource,
            const ComponentVariables& inVariables,
            const ComponentFunctions& inFunctions
        )
            : m_id(inId),
            m_variables(inVariables),
            m_functions(inFunctions),
            m_wasStyleAdded(false)
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

            validate(m_document.first_child());
        }

        View::View(const std::string& inId)
            : m_id(inId),
            m_variables({}),
            m_functions({}),
            m_wasStyleAdded(false)
        {}

        const std::string& View::getId() const
        {
            return m_id;
        }

        void View::rebuild()
        {
            m_wasStyleAdded = false;
        }

        void View::show(
            const Vec<2, int>& inResolution,
            const Vec<2, int>& inPosition
        )
        {
            compile(
                m_document.first_child(),
                inResolution,
                inPosition
            );
        }

        bool View::hasVariable(const std::string& inId) const
        {
            return m_variables.find(inId) != m_variables.end() && m_variables.at(inId) != nullptr;
        }

        const ComponentVariable View::getVariable(const std::string& inId) const
        {
            if (!hasVariable(inId))
            {
                return nullptr;
            }

            return m_variables.at(inId);
        }

        void View::addVariable(const ComponentVariables& inVariables)
        {
            for (auto [id, variable] : inVariables)
            {
                addVariable(id, variable);
            }
        }

        void View::addVariable(const std::string& inId, ComponentVariable inVariable)
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

        const ComponentFunction View::getFunction(const std::string& inId) const
        {
            if (!hasFunction(inId))
            {
                return {};
            }

            return m_functions.at(inId);
        }

        void View::addFunction(const ComponentFunctions& inFunctions)
        {
            for (auto [id, function] : inFunctions)
            {
                addFunction(id, function);
            }
        }

        void View::addFunction(const std::string& inId, ComponentFunction inFunction)
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

        void View::validate(const pugi::xml_node& inNode)
        {
            bool isRoot  = inNode.parent() == inNode.root();
            bool isAlone = isRoot && inNode.next_sibling() == nullptr;

            if (!isRoot || !isAlone)
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            if (TAG_ID.compare(inNode.name()) != 0)
            {
                throw std::runtime_error("UI document root element must be a " + TAG_ID);
            }
        }

        void View::compile(
            const pugi::xml_node& inNode,
            const Vec<2, int>& inResolution,
            const Vec<2, int>& inPosition
        )
        {
            ImGuiWindowFlags viewFlags = ImGuiWindowFlags_NoNav |
                                         ImGuiWindowFlags_NoDecoration |
                                         ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoBackground;

            ImVec2 uiResolution = ImVec2(
                static_cast<float>(inResolution.x),
                static_cast<float>(inResolution.y)
            );
            ImVec2 uiPosition = ImVec2(
                static_cast<float>(inPosition.x),
                static_cast<float>(inPosition.y)
            );

            ImGui::SetNextWindowContentSize(uiResolution);
            ImGui::SetNextWindowSize(uiResolution);
            ImGui::SetNextWindowPos(uiPosition);

            if (!m_wasStyleAdded)
            {
                addStyle(inNode);

                m_wasStyleAdded = true;
            }

            execOnTick(inNode);

            ImGui::Begin(
                m_id.c_str(),
                nullptr,
                viewFlags
            );
                compileChildren(inNode);
            ImGui::End();
        }
    }
}