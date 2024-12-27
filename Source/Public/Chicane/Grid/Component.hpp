#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            typedef std::any* Variable;
            typedef std::unordered_map<std::string, Variable> Variables;

            typedef pugi::xml_node_iterator Child;
            typedef pugi::xml_object_range<Child> Children;

            typedef std::function<void (const pugi::xml_node&)> Compiler;

            struct FunctionData
            {
            public:
                std::string name             = "";
                std::vector<std::any> params {};

            public:
                static const FunctionData& empty();
            };

            struct Event
            {
                std::vector<std::any> values {};
            };

            typedef std::function<std::any (const Event&)> Function;
            typedef std::unordered_map<std::string, Function> Functions;

            struct BaseProps
            {
            public:
                bool                             bIsVisible = true; // Optional
                std::string                      id         = ""; // Required
                Style                            style      = {}; // Optional
                Component::Children              children   = Component::Children(pugi::xml_node_iterator(), pugi::xml_node_iterator()); // Optional
                std::vector<Component::Function> _renderers = {}; // [Internal Use]
            };
        }

        bool hasComponent(const std::string& inId);
        void addComponent(const std::string& inId, Component::Compiler inCompiler);
        Component::Compiler getComponent(const std::string& inId);
    }
}