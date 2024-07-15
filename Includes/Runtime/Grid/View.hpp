#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Grid/Essential.hpp"

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
                const ComponentVariables& inVariables = {},
                const ComponentFunctions& inFunctions = {} 
            );
            View(const std::string& inId);
            virtual ~View() = default;

        public:
            std::string getId();

            virtual void show(
                const Vec2& inResolution,
                const Vec2& inPosition
            );

            bool hasVariable(const std::string& inId);
            ComponentVariable getVariable(const std::string& inId);
            void addVariable(const ComponentVariables& inVariables);
            void addVariable(const std::string& inId, ComponentVariable inVariable);
            void removeVariable(const std::string& inId);

            bool hasFunction(const std::string& inId);
            ComponentFunction getFunction(const std::string& inId);
            void addFunction(const ComponentFunctions& inFunctions);
            void addFunction(const std::string& inId, ComponentFunction inFunction);
            void removeFunction(const std::string& inId);

        protected:
            void validate(const pugi::xml_node& inNode);
            void compile(
                pugi::xml_node& outNode,
                const Vec2& inResolution,
                const Vec2& inPosition
            );

        protected:
            std::string m_id;
            pugi::xml_document m_document;
            ComponentVariables m_variables;
            ComponentFunctions m_functions;
        
        private:
            const std::string TAG_ID = "View";
        };
    }
}