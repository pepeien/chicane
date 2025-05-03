#pragma once

#include "Core.hpp"
#include "Grid/Function.hpp"
#include "Grid/Reference.hpp"
#include "Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE View : public Component
        {
        public:
            View(
                const std::string& inId,
                const std::string& inSource,
                const References& inReferences = {},
                const Functions& inFunctions = {} 
            );
            View(const std::string& inId);

            virtual ~View() = default;

        public:
            virtual void show(const Vec<2, int>& inSize, const Vec<2, int>& inPosition);

        public:
            void activate();
            void rebuild();

        protected:
            void compile(
                const pugi::xml_node& inNode,
                const Vec<2, int>& inSize,
                const Vec<2, int>& inPosition
            );

        protected:
            pugi::xml_document m_document;

        private:
            bool m_bWasStyleAdded;
        };
    }
}