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
                const ComponentFunctions& inCallbacks = {} 
            );
            View(const std::string& inId);
            virtual ~View() = default;

        public:
            std::string getId();

            virtual void show(
                const Vec2& inResolution,
                const Vec2& inPosition
            );

            bool hasFunction(const std::string& inId);
            ComponentFunction getFunction(const std::string& inId);
            void addFunction(const ComponentFunctions& inCallbacks);
            void addFunction(const std::string& inId, ComponentFunction inCallback);
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
            ComponentFunctions m_callbacks;
        
        private:
            const std::string TAG_ID = "View";
        };
    }
}