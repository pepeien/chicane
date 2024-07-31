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

        struct ComponentMargin
        {
            float top    = 0.0f;
            float bottom = 0.0f;
            float left   = 0.0f;
            float right  = 0.0f;
        };

        enum class ComponentPosition : std::uint8_t
        {
            Absolute,
            Relative
        };

        typedef std::function<std::any (const ComponentEvent&)> ComponentFunction;
        typedef std::unordered_map<std::string, ComponentFunction> ComponentFunctions;

        typedef std::any* ComponentVariable;
        typedef std::unordered_map<std::string, ComponentVariable> ComponentVariables;

        // Ref Value
        constexpr auto REF_VALUE_OPENING = "{{";
        constexpr auto REF_VALUE_CLOSING = "}}";

        constexpr char FUNCTION_PARAMS_OPENING = '(';
        constexpr char FUNCTION_PARAMS_CLOSING = ')';

        // Value attributes
        constexpr auto ID_ATTRIBUTE_NAME     = "id";
        constexpr auto WIDTH_ATTRIBUTE_NAME  = "width";
        constexpr auto HEIGHT_ATTRIBUTE_NAME = "height";

        constexpr auto POSITION_ATTRIBUTE_NAME = "position";
        constexpr auto POSITION_TYPE_ABSOLUTE  = "absolute";
        constexpr auto POSITION_TYPE_RELATIVE  = "relative";

        constexpr char MARGIN_SEPARATOR = ' ';
        /*
            Template 1: "`SINGLE_MARGIN`"
            Template 2: "`VERTICAL_MARGIN` `HORIZONTAL_MARGIN`"
            Template 3: "`TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`"
            Template 4: "`TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`"
        */
        constexpr auto MARGIN_ATTRIBUTE_NAME        = "margin";
        constexpr auto MARGIN_TOP_ATTRIBUTE_NAME    = "margin-top";
        constexpr auto MARGIN_BOTTOM_ATTRIBUTE_NAME = "margin-bottom";
        constexpr auto MARGIN_LEFT_ATTRIBUTE_NAME   = "margin-left";
        constexpr auto MARGIN_RIGHT_ATTRIBUTE_NAME  = "margin-right";

        // Lifecycle attributes
        constexpr auto ON_TICK_ATTRIBUTE = "onTick";

        // Methods
        bool endsWith(const std::string& inTarget, const std::string& inEnding);

        std::string getTag(const pugi::xml_node& inNode);

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

        ComponentMargin getMargin(const pugi::xml_node& inNode);
        ComponentPosition getPosition(const pugi::xml_node& inNode);

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
    }
}