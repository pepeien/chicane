#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Grid/Component/Function.hpp"
#include "Chicane/Grid/Component/Variable.hpp"

namespace Chicane
{
    namespace Grid
    {
        class View
        {
        public:
            View(
                const std::string& inId,
                const std::string& inSource,
                const Component::Variables& inVariables = {},
                const Component::Functions& inFunctions = {} 
            );
            View(const std::string& inId);
            virtual ~View() = default;

        public:
            virtual void show(
                const Vec<2, int>& inResolution,
                const Vec<2, int>& inPosition
            );

        public:
            const std::string& getId() const;

            void activate();
            void rebuild();

            bool hasVariable(const std::string& inId) const;
            Component::Variable getVariable(const std::string& inId) const;
            void addVariable(const Component::Variables& inVariables);
            void addVariable(const std::string& inId, Component::Variable inVariable);
            void removeVariable(const std::string& inId);

            bool hasFunction(const std::string& inId) const;
            const Component::Function getFunction(const std::string& inId) const;
            void addFunction(const Component::Functions& inFunctions);
            void addFunction(const std::string& inId, Component::Function inFunction);
            void removeFunction(const std::string& inId);

        protected:
            void validate(const pugi::xml_node& inNode);
            void compile(
                const pugi::xml_node& inNode,
                const Vec<2, int>& inResolution,
                const Vec<2, int>& inPosition
            );

        protected:
            std::string m_id;
            pugi::xml_document m_document;
            Component::Variables m_variables;
            Component::Functions m_functions;

        private:
            bool m_bWasStyleAdded;

            ImGuiWindowFlags m_flags;

            ImVec2 m_resolution;
            ImVec2 m_position;
        };
    }
}