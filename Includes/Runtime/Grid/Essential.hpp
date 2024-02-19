#pragma once

#include "Runtime/Runtime.hpp"

#include <functional>

namespace Chicane
{
    namespace Grid
    {
        class View;

        typedef std::function<void (pugi::xml_node&)> ComponentCallback;
        typedef std::unordered_map<std::string, ComponentCallback> ComponentCallbackMap;
        typedef std::unordered_map<std::string, ComponentCallbackMap> ViewCallbackMap;

        // Value attributes
        const std::string ID_ATTRIBUTE_NAME     = "id";
        const std::string WIDTH_ATTRIBUTE_NAME  = "width";
        const std::string HEIGHT_ATTRIBUTE_NAME = "height";

        // Lifecycle attributes
        const std::string ON_TICK_ATTRIBUTE = "onTick";

        bool endsWith(const std::string& inTarget, const std::string& inEnding);

        std::string getTag(const pugi::xml_node& outNode);

        float getSizeFromPixel(const pugi::xml_attribute& inAttribute);

        float calculateSizeFromViewportHeight(float inVhValue);
        float getSizeFromViewportHeight(const pugi::xml_attribute& inAttribute);

        float calculateSizeFromViewportWidth(float inVwValue);
        float getSizeFromViewportWidth(const pugi::xml_attribute& inAttribute);

        float getSize(
            const std::string& inAttributeName,
            const pugi::xml_node& inNode
        );

        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& outNode
        );

        bool hasViews();
        void addView(View* inView);
        View* getActiveView();
        void setActiveView(const std::string& inViewID);
        
        void compileChildren(pugi::xml_node& outNode);
        void compileChild(pugi::xml_node& outNode);
    }
}