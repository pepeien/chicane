#pragma once

#include "Core.hpp"
#include "Grid/Component.hpp"
#include "Grid/Function.hpp"
#include "Grid/Reference.hpp"
#include "Grid/Style.hpp"
#include "Grid/View.hpp"

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
        CHICANE bool endsWith(const std::string& inTarget, const std::string& inEnding);

        CHICANE Vec<4, std::uint32_t> hexToRgba(const std::string& inColor);
        CHICANE Vec<3, std::uint32_t> hexToRgb(const std::string& inColor);

        CHICANE float getSizeFromPixel(const pugi::xml_attribute& inAttribute);

        CHICANE float calculateSizeFromViewportHeight(float inVhValue);
        CHICANE float getSizeFromViewportHeight(const pugi::xml_attribute& inAttribute);

        CHICANE float calculateSizeFromViewportWidth(float inVwValue);
        CHICANE float getSizeFromViewportWidth(const pugi::xml_attribute& inAttribute);

        CHICANE float getSize(
            const std::string& inValue,
            Style::Direction inDirection = Style::Direction::Horizontal,
            Style::Position inPosition = Style::Position::Absolute
        );
        CHICANE float getSize(
            const std::string& inAttributeName,
            const pugi::xml_node& inNode
        );

        CHICANE void execOnTick(const pugi::xml_node& inNode);

        CHICANE void compileChildren(const pugi::xml_node& inNode);
        CHICANE void compileChild(const pugi::xml_node& inNode);

        CHICANE std::string anyToString(const Reference& inValue);

        CHICANE FunctionData parseFunction(const std::string& inRefValue);

        CHICANE bool doesTextContainsReference(const std::string& inText);
        CHICANE bool doesTextContainsFunction(const std::string& inValue);
        CHICANE Reference parseReference(const std::string& inValue);
        CHICANE std::string parseText(const std::string& inText);

        CHICANE std::vector<Reference> getItems(const pugi::xml_node& inNode);
        CHICANE Function getItemGetter(const pugi::xml_node& inNode);

        CHICANE bool hasView();
        CHICANE void addView(View* inView);
        CHICANE void setView(const std::string& inId);
        CHICANE View* getView(const std::string& inId = "");
        CHICANE Subscription<View*>* watchView(
            std::function<void (View*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );
    }
}