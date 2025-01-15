#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Reference.hpp"
#include "Chicane/Grid/Style.hpp"
#include "Chicane/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        // Ref Value
        static constexpr const char* REF_VALUE_OPENING = "{{";
        static constexpr const char* REF_VALUE_CLOSING = "}}";

        static constexpr char FUNCTION_PARAMS_OPENING = '(';
        static constexpr char FUNCTION_PARAMS_CLOSING = ')';

        // Value attributes
        static constexpr const char* IS_VISIBLE_ATTRIBUTE_NAME = "isVisible";
        static constexpr const char* ID_ATTRIBUTE_NAME         = "id";

        // Lifecycle attributes
        static constexpr const char* ON_TICK_ATTRIBUTE = "onTick";

        // List
        static constexpr const char* ITEMS_ATTRIBUTE_NAME       = "items";
        static constexpr const char* ITEM_GETTER_ATTRIBUTE_NAME = "itemGetter";

        // Size units
        static constexpr const char* AUTO_SIZE_UNIT            = "auto";
        static constexpr const char* PIXEL_SIZE_UNIT           = "px";
        static constexpr const char* PERCENTAGE_SIZE_UNIT      = "%";
        static constexpr const char* VIEWPORT_HEIGHT_SIZE_UNIT = "vh";
        static constexpr const char* VIEWPORT_WIDTH_SIZE_UNIT  = "vw";

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

        void execOnTick(const pugi::xml_node& inNode);

        void compileChildren(const pugi::xml_node& inNode);
        void compileChild(const pugi::xml_node& inNode);

        std::string anyToString(const Reference& inValue);

        Component::FunctionData parseFunction(const std::string& inRefValue);

        bool doesTextContainsReference(const std::string& inText);
        bool doesTextContainsFunction(const std::string& inValue);
        Reference parseReference(const std::string& inValue);
        std::string parseText(const std::string& inText);

        // List
        std::vector<Reference> getItems(const pugi::xml_node& inNode);
        Component::Function getItemGetter(const pugi::xml_node& inNode);
    }
}