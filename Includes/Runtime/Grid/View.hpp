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
            View(const std::string& inId);
            View(const std::string& inId, const std::string inSource);
            virtual ~View() = default;

        public:
            std::string getId();

            virtual void show(
                const Vec2& inResolution,
                const Vec2& inPosition
            );

            bool hasCallback(const std::string& inId);
            void addCallback(const ComponentCallbackMap& inCallbacks);
            void addCallback(const std::string& inId, ComponentCallback inCallback);
            void removeCallback(const std::string& inId);
            void execCallback(const std::string& inId, pugi::xml_node& outNode);

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
            ComponentCallbackMap m_callbacks;
        
        private:
            const std::string TAG_ID = "View";
        };
    }
}