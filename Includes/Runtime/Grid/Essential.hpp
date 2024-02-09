#pragma once

#include "Runtime/Runtime.hpp"

namespace Chicane
{
    namespace Grid
    {
        typedef std::unordered_map<std::string, void (*)(pugi::xml_node& outNode)> ComponentCallbackMap;
        typedef std::unordered_map<std::string, ComponentCallbackMap> ViewCallbackMap;

        // Value attributes
        const std::string ID_ATTRIBUTE_NAME     = "id";
        const std::string WIDTH_ATTRIBUTE_NAME  = "width";
        const std::string HEIGHT_ATTRIBUTE_NAME = "height";

        // Lifecycle attributes
        const std::string ON_TICK_ATTRIBUTE = "onTick";

        struct View
        {
            ComponentCallbackMap callbacks;
        };

        bool endsWith(const std::string& inTarget, const std::string& inEnding);

        std::string getTag(const pugi::xml_node& outNode);

        float getSizeFromPixel(const pugi::xml_attribute& inAttribute);
        float getSizeFromViewportHeight(const pugi::xml_attribute& inAttribute);
        float getSizeFromViewportWidth(const pugi::xml_attribute& inAttribute);
        float getSize(
            const std::string& inAttributeName,
            const pugi::xml_node& inNode
        );

        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& outNode
        );

        View getActiveView();
        void setActiveView(const View& inView);
        
        void compileChildren(pugi::xml_node& outNode);
        void compileChild(pugi::xml_node& outNode);
    }
}