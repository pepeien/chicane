#pragma once

#include "Chicane/Base.hpp"

#include <any>
#include <functional>

namespace Chicane
{
    namespace Grid
    {
        class View;

        // Types
        struct ComponentFunctionData {
            std::string name             = "";
            std::vector<std::any> params = {};
        };
        struct ComponentEvent {
            std::vector<std::any> values = {};
        };

        typedef std::function<std::any (ComponentEvent)> ComponentFunction;
        typedef std::unordered_map<std::string, ComponentFunction> ComponentFunctions;

        typedef std::any* ComponentVariable;
        typedef std::unordered_map<std::string, ComponentVariable> ComponentVariables;

        // Ref Value
        const std::string REF_VALUE_OPENING = "{{";
        const std::string REF_VALUE_CLOSING = "}}";

        const char FUNCTION_PARAMS_OPENING = '(';
        const char FUNCTION_PARAMS_CLOSING = ')';

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

        void execOnTick(const pugi::xml_node& inNode);

        void compileChildren(pugi::xml_node& outNode);
        void compileChild(pugi::xml_node& outNode);

        std::string anyToString(const std::any& inValue);

        ComponentFunctionData parseFunction(const std::string& inRefValue);

        bool textContainsRefValue(const std::string& inText);
        bool refValueContainsFunction(const std::string& inRefValue);
        std::any processRefValue(const std::string& inRefValue);
        std::string processText(const std::string& inText);
    }
}