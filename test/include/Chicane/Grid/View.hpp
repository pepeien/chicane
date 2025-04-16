#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math.hpp"
#include "Component/Function.hpp"
#include "Component/Variable.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE View
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
                const Vec<2, int>& inSize,
                const Vec<2, int>& inPosition
            );

        public:
            void activate();
            void rebuild();

            const std::string& getId() const;

            const Vec<2, int>& getSize() const;
            void setSize(const Vec<2, int>& inSize);
            void setSize(int inWidth, int inHeight);

            const Vec<2, int>& getPosition() const;
            void setPosition(const Vec<2, int>& inPosition);
            void setPosition(int inX, int inY);

            bool hasVariable(const std::string& inId) const;
            Reference* getVariable(const std::string& inId) const;
            void addVariable(const Component::Variables& inVariables);
            void addVariable(const std::string& inId, Reference* inVariable);
            void removeVariable(const std::string& inId);

            bool hasFunction(const std::string& inId) const;
            const Component::Function getFunction(const std::string& inId) const;
            void addFunction(const Component::Functions& inFunctions);
            void addFunction(const std::string& inId, Component::Function inFunction);
            void removeFunction(const std::string& inId);

        protected:
            void assertProps(const pugi::xml_node& inNode);
            void compile(
                const pugi::xml_node& inNode,
                const Vec<2, int>& inSize,
                const Vec<2, int>& inPosition
            );

        protected:
            std::string          m_id;
            pugi::xml_document   m_document;
            Component::Variables m_variables;
            Component::Functions m_functions;

        private:
            bool        m_bWasStyleAdded;

            Vec<2, int> m_size;
            Vec<2, int> m_position;
        };
    }
}