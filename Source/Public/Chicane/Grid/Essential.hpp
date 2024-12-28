#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Style.hpp"
#include "Chicane/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        // Ref Value
        static constexpr auto REF_VALUE_OPENING = "{{";
        static constexpr auto REF_VALUE_CLOSING = "}}";

        constexpr char FUNCTION_PARAMS_OPENING = '(';
        constexpr char FUNCTION_PARAMS_CLOSING = ')';

        // Value attributes
        static constexpr auto IS_VISIBLE_ATTRIBUTE_NAME = "isVisible";
        static constexpr auto ID_ATTRIBUTE_NAME         = "id";

        // Lifecycle attributes
        static constexpr auto ON_TICK_ATTRIBUTE = "onTick";

        // List
        static constexpr auto ITEMS_ATTRIBUTE_NAME       = "items";
        static constexpr auto ITEM_GETTER_ATTRIBUTE_NAME = "itemGetter";

        // Size units
        static constexpr auto AUTO_SIZE_UNIT            = "auto";
        static constexpr auto PIXEL_SIZE_UNIT           = "px";
        static constexpr auto PERCENTAGE_SIZE_UNIT      = "%";
        static constexpr auto VIEWPORT_HEIGHT_SIZE_UNIT = "vh";
        static constexpr auto VIEWPORT_WIDTH_SIZE_UNIT  = "vw";

        // Methods
        bool endsWith(const std::string& inTarget, const std::string& inEnding);

        ImVec4 hexToImGuiColor(const std::string& inColor);
        Vec<4, std::uint32_t> hexToRgba(const std::string& inColor);
        Vec<3, std::uint32_t> hexToRgb(const std::string& inColor);

        std::uint32_t getChildrenCount(const Component::Children& inChildren);
        std::vector<pugi::xml_node> extractChildren(const Component::Children& inChildren);

        float getSizeFromPixel(const pugi::xml_attribute& inAttribute);

        float calculateSizeFromViewportHeight(float inVhValue);
        float getSizeFromViewportHeight(const pugi::xml_attribute& inAttribute);

        float calculateSizeFromViewportWidth(float inVwValue);
        float getSizeFromViewportWidth(const pugi::xml_attribute& inAttribute);

        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        );

        float getSize(
            const std::string& inValue,
            Style::Direction inDirection = Style::Direction::Horizontal,
            Style::Position inPosition = Style::Position::Absolute
        );
        float getSize(
            const std::string& inAttributeName,
            const pugi::xml_node& inNode
        );

        bool hasViews();
        void addView(View* inView);
        void addView(const std::vector<View*>& inViews);
        View* getActiveView();
        std::string getActiveViewId();
        void setActiveView(const std::string& inViewID);

        void execOnTick(const pugi::xml_node& inNode);

        void compileChildren(const pugi::xml_node& inNode);
        void compileChild(const pugi::xml_node& inNode);

        std::string anyToString(const std::any& inValue);

        Component::FunctionData parseFunction(const std::string& inRefValue);

        bool textContainsRefValue(const std::string& inText);
        bool refValueContainsFunction(const std::string& inRefValue);
        std::any processRefValue(const std::string& inRefValue);
        std::string processText(const std::string& inText);

        // List
        std::vector<std::any> getItems(const pugi::xml_node& inNode);
        Component::Function getItemGetter(const pugi::xml_node& inNode);
    }
}