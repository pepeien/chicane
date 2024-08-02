#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Grid
    {
        class View;

        // Types
        struct ComponentFunctionData
        {
            std::string name             = "";
            std::vector<std::any> params {};
        };

        struct ComponentEvent
        {
            std::vector<std::any> values {};
        };

        typedef std::function<std::any (const ComponentEvent&)> ComponentFunction;
        typedef std::unordered_map<std::string, ComponentFunction> ComponentFunctions;

        typedef std::any* ComponentVariable;
        typedef std::unordered_map<std::string, ComponentVariable> ComponentVariables;

        typedef pugi::xml_node_iterator ComponentChild;
        typedef pugi::xml_object_range<ComponentChild> ComponentChildren;

        // Ref Value
        constexpr auto REF_VALUE_OPENING = "{{";
        constexpr auto REF_VALUE_CLOSING = "}}";

        constexpr char FUNCTION_PARAMS_OPENING = '(';
        constexpr char FUNCTION_PARAMS_CLOSING = ')';

        // Value attributes
        constexpr auto ID_ATTRIBUTE_NAME = "id";

        // Lifecycle attributes
        constexpr auto ON_TICK_ATTRIBUTE = "onTick";

        // List
        constexpr auto ITEMS_ATTRIBUTE_NAME       = "items";
        constexpr auto ITEM_GETTER_ATTRIBUTE_NAME = "item-getter";

        // Methods
        bool endsWith(const std::string& inTarget, const std::string& inEnding);

        std::string getTag(const pugi::xml_node& inNode);

        ImVec4 hexToColor(const std::string& inColor);

        std::uint32_t getChildrenCount(const ComponentChildren& inChildren);
        std::vector<pugi::xml_node> extractChildren(const ComponentChildren& inChildren);

        float getSizeFromPixel(const pugi::xml_attribute& inAttribute);

        float calculateSizeFromViewportHeight(float inVhValue);
        float getSizeFromViewportHeight(const pugi::xml_attribute& inAttribute);

        float calculateSizeFromViewportWidth(float inVwValue);
        float getSizeFromViewportWidth(const pugi::xml_attribute& inAttribute);

        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        );

        float getSize(const std::string inValue);
        float getSize(
            const std::string& inAttributeName,
            const pugi::xml_node& inNode
        );

        bool hasViews();
        void addView(View* inView);
        void addView(const std::vector<View*>& inViews);
        View* getActiveView();
        void setActiveView(const std::string& inViewID);

        void execOnTick(const pugi::xml_node& inNode);

        void compileChildren(const pugi::xml_node& inNode);
        void compileChild(const pugi::xml_node& inNode);

        std::string anyToString(const std::any& inValue);

        ComponentFunctionData parseFunction(const std::string& inRefValue);

        bool textContainsRefValue(const std::string& inText);
        bool refValueContainsFunction(const std::string& inRefValue);
        std::any processRefValue(const std::string& inRefValue);
        std::string processText(const std::string& inText);

        // List
        std::vector<std::any> getItems(const pugi::xml_node& inNode);
        ComponentFunction getItemGetter(const pugi::xml_node& inNode);
    }
}