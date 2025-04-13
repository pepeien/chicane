#include "Chicane/Grid/Essential.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        static const std::vector<Reference> EMPTY_ITEMS = {};

        std::unordered_map<std::string, Vec<4, std::uint32_t>> m_rgbaColors {
            { HEX_COLOR_TRANSPARENT, Vec<4, std::uint32_t>(0) }
        };
        std::unordered_map<std::string, Vec<3, std::uint32_t>> m_rgbColors {
            { HEX_COLOR_TRANSPARENT, Vec<3, std::uint32_t>(0) }
        };

        bool endsWith(const std::string& inTarget, const std::string& inEnding)
        {
            if (inTarget.empty() || inTarget.size() < inEnding.size())
            {
                return false;
            }

            return String::areEquals(std::string(inTarget.end() - inEnding.size(), inTarget.end()), inEnding);
        }

        Vec<4, std::uint32_t> hexToRgba(const std::string& inColor)
        {
            std::string backgroundColor = String::trim(inColor);
            std::transform(
                backgroundColor.begin(),
                backgroundColor.end(),
                backgroundColor.begin(),
                ::tolower
            );

            bool bIsTransparent = backgroundColor.empty() || String::areEquals(backgroundColor, BACKGROUND_COLOR_TRANSPARENT);
            bool bIsNotHex      = backgroundColor.size() < 7 || backgroundColor.size() > 9;

            if (bIsTransparent || bIsNotHex)
            {
                backgroundColor = HEX_COLOR_TRANSPARENT;
            }

            if (m_rgbaColors.find(backgroundColor) == m_rgbaColors.end())
            {
                std::string hexColor = backgroundColor;

                backgroundColor = backgroundColor.substr(
                    1,
                    backgroundColor.size() - 1
                );
                backgroundColor = backgroundColor.size() == 6 ? backgroundColor + "FF" : backgroundColor;

                Vec<4, std::uint32_t> result = Vec<4, std::uint32_t>(0U);
                
                sscanf(
                    backgroundColor.c_str(),
                    "%02hx%02x%02hx%02hx",
                    &result.r,
                    &result.g,
                    &result.b,
                    &result.a
                );

                m_rgbaColors.insert(
                    std::make_pair(
                        hexColor,
                        result
                    )
                );

                return result;
            }

            return m_rgbaColors.at(backgroundColor);
        }

        Vec<3, std::uint32_t> hexToRgb(const std::string& inColor)
        {
            Vec<4, std::uint32_t> color = hexToRgba(inColor);

            if (m_rgbColors.find(inColor) == m_rgbColors.end())
            {
                Vec<3, std::uint32_t> result(color.x, color.y, color.z);

                m_rgbColors.insert(
                    std::make_pair(
                        inColor,
                        result
                    )
                );

                return result;
            }

            return m_rgbColors.at(inColor);
        }

        std::uint32_t getChildrenCount(const Component::Children& inChildren)
        {
            std::uint32_t result = 0;

            for (const Component::Child& child : inChildren)
            {
                result++;
            }

            return result;
        }

        std::vector<pugi::xml_node> extractChildren(const Component::Children& inChildren)
        {
            std::vector<pugi::xml_node> result = {};

            for (const Component::Child& child : inChildren)
            {
                result.push_back(*child);
            }

            return result;
        }

        float getSizeFromPixel(const std::string& inValue)
        {
            if (!endsWith(inValue, PIXEL_SIZE_UNIT))
            {
                return 0.0f;
            }
        
            return std::stof(std::string(inValue.begin(), inValue.end() - 2));
        }

        float getSizeFromPixel(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            return getSizeFromPixel(inAttribute.as_string());
        }

        float getSizeFromPercentage(
            const std::string& inValue,
            Style::Direction inDirection,
            Style::Position inPosition
        )
        {
            if (!endsWith(inValue, PERCENTAGE_SIZE_UNIT))
            {
                return 0.0f;
            }

            float percentage = std::stof(std::string(inValue.begin(), inValue.end() - 1)) / 100;

            Vec<2, std::uint32_t> regionSize = Vec<2, std::uint32_t>(0U);

            if (inDirection == Style::Direction::Horizontal)
            {
                return regionSize.x * percentage;
            }

            return regionSize.y * percentage;
        }

        float calculateSizeFromViewportHeight(float inVhValue)
        {
            return Application::getWindow()->getSize().y * (inVhValue / 100);
        }

        float getSizeFromViewportHeight(const std::string& inValue)
        {
            if (!endsWith(inValue, VIEWPORT_HEIGHT_SIZE_UNIT))
            {
                return 0.0f;
            }

            return calculateSizeFromViewportHeight(
                std::stof(std::string(inValue.begin(), inValue.end() - 2))
            );
        }

        float getSizeFromViewportHeight(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            return getSizeFromViewportHeight(inAttribute.as_string());
        }

        float calculateSizeFromViewportWidth(float inVwValue)
        {
            return Application::getWindow()->getSize().x * (inVwValue / 100);
        }

        float getSizeFromViewportWidth(const std::string& inValue)
        {
            if (!endsWith(inValue, VIEWPORT_WIDTH_SIZE_UNIT))
            {
                return 0.0f;
            }

            return calculateSizeFromViewportWidth(
                std::stof(std::string(inValue.begin(), inValue.end() - 2))
            );
        }

        float getSizeFromViewportWidth(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            return getSizeFromViewportWidth(inAttribute.as_string());
        }

        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        )
        {
            return inNode.attribute(inName.c_str());
        }

        float getSize(const std::string& inValue, Style::Direction inDirection, Style::Position inPosition)
        {
            if (inValue.empty())
            {
                return 0.0f;
            }

            if (String::areEquals(inValue, AUTO_SIZE_UNIT))
            {
                return getSizeFromPercentage("100%", inDirection, inPosition);
            }

            if (endsWith(inValue, PIXEL_SIZE_UNIT))
            {
                return getSizeFromPixel(inValue);
            }

            if (endsWith(inValue, PERCENTAGE_SIZE_UNIT))
            {
                return getSizeFromPercentage(inValue, inDirection, inPosition);
            }

            if (endsWith(inValue, VIEWPORT_HEIGHT_SIZE_UNIT))
            {
                return getSizeFromViewportHeight(inValue);
            }

            if (endsWith(inValue, VIEWPORT_WIDTH_SIZE_UNIT))
            {
                return getSizeFromViewportWidth(inValue);
            }

            std::string parsedValue = anyToString(parseReference(inValue));

            if (parsedValue.empty())
            {
                return 0.0f;
            }

            return std::stof(parsedValue);
        }

        float getSize(
            const std::string& inAttributeName,
            const pugi::xml_node& inNode
        )
        {
            pugi::xml_attribute attributeValue = getAttribute(inAttributeName, inNode);

            if (attributeValue.empty())
            {
                return 0.0f;
            }

            return getSize(attributeValue.as_string());
        }

        void execOnTick(const pugi::xml_node& inNode)
        {
            pugi::xml_attribute onTickFunction = getAttribute(ON_TICK_ATTRIBUTE, inNode);

            if (onTickFunction.empty())
            {
                return;
            }

            parseReference(onTickFunction.as_string());
        }

        void compileChildren(const pugi::xml_node& inNode)
        {
            for (const pugi::xml_node& child : inNode.children())
            {
                compileChild(child);
            }
        }

        void compileChild(const pugi::xml_node& inNode)
        {
            if (!hasComponent(inNode.name()))
            {
                return;
            }

            const Style& style = Style::getStyle(inNode);

            if (style.display == Style::Display::None)
            {
                return;
            }
/*
            ImVec2 position = style.position == Style::Position::Relative ? ImGui::GetCursorPos() : START_POSITION;

            if (style.margin.left != 0.0f || style.margin.right != 0.0f)
            {
                if (style.margin.left == style.margin.right)
                {
                    position.x += style.margin.left;
                }
                else
                {
                    position.x += style.margin.left - style.margin.right;
                }
            }

            if (style.margin.top != 0.0f || style.margin.bottom != 0.0f)
            {
                if (style.margin.top == style.margin.bottom)
                {
                    position.y += style.margin.top;
                }
                else
                {
                    position.y += style.margin.top - style.margin.bottom;
                }
            }

            ImGui::SetCursorPos(position);

            if (style.display == Style::Display::Hidden)
            {
                return;
            }

            execOnTick(inNode);

            getComponent(inNode.name())(inNode);
*/
        }

        std::string anyToString(const Reference& inValue)
        {
            if (inValue.isType<std::string>())
            {
                return *inValue.getValue<std::string>();
            }

            if (inValue.isType<bool>())
            {
                return std::to_string(*inValue.getValue<bool>());
            }

            if (inValue.isType<int>())
            {
                return std::to_string(*inValue.getValue<int>());
            }

            if (inValue.isType<std::uint64_t>())
            {
                return std::to_string(*inValue.getValue<std::uint64_t>());
            }

            if (inValue.isType<std::uint32_t>())
            {
                return std::to_string(*inValue.getValue<std::uint32_t>());
            }

            if (inValue.isType<float>())
            {
                return std::to_string(*inValue.getValue<float>());
            }

            return "";
        }

        Component::FunctionData parseFunction(const std::string& inRefValue)
        {
            std::string trimmedValue = String::trim(inRefValue);

            if (trimmedValue.empty())
            {
                return Component::FunctionData::empty();
            }

            std::size_t paramsStart = trimmedValue.find_first_of(FUNCTION_PARAMS_OPENING);
            std::size_t paramsEnd   = trimmedValue.find_last_of(FUNCTION_PARAMS_CLOSING);
            std::string name        = trimmedValue.substr(0, paramsStart);

            paramsStart += 1;
            paramsEnd   -= paramsStart;

            std::string params = String::trim(
                inRefValue.substr(
                    paramsStart,
                    paramsEnd
                )
            );

            Component::FunctionData data = {};
            data.name = name;

            for (std::string& value : String::split(params, ','))
            {
                data.params.push_back(
                    parseReference(String::trim(value))
                );
            }

            return data;
        }

        bool doesTextContainsReference(const std::string& inText)
        {
            bool hasOpening = inText.find_first_of(REF_VALUE_OPENING) != std::string::npos;
            bool hasClosing = inText.find_first_of(REF_VALUE_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        bool doesTextContainsFunction(const std::string& inValue)
        {
            bool hasOpening = inValue.find_first_of(FUNCTION_PARAMS_OPENING) != std::string::npos;
            bool hasClosing = inValue.find_first_of(FUNCTION_PARAMS_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        Reference parseReference(const std::string& inValue)
        {
            if (!Application::hasView())
            {
                return Reference::fromValue<const std::string>(&inValue);
            }

            View* view = Application::getView();

            if (!doesTextContainsFunction(inValue))
            {
                if (!view->hasVariable(inValue))
                {
                    return Reference::fromValue<const std::string>(&inValue);
                }

                return *view->getVariable(inValue);
            }

            Component::FunctionData functionData = parseFunction(inValue);
            Component::Function functionRef      = view->getFunction(functionData.name);

            if (!functionRef)
            {
                return Reference::fromValue<const std::string>(&inValue);
            }

            Component::Event event = {};
            event.values = functionData.params;

            return functionRef(event);
        }

        std::string parseText(const std::string& inText)
        {
            if (String::trim(inText).empty())
            {
                return "";
            }

            if (!doesTextContainsReference(inText))
            {
                return inText;
            }

            std::size_t foundOpening = inText.find_first_of(REF_VALUE_OPENING);
            std::size_t foundClosing = inText.find_first_of(REF_VALUE_CLOSING);
            std::string resultText = inText.substr(0,  foundOpening);
            std::string remainderText = inText.substr(
                foundClosing + 2,
                inText.size() - foundClosing
            );

            foundOpening += 2;
            foundClosing -= foundOpening;

            std::string value = inText.substr(foundOpening, foundClosing);
            value = String::trim(value);

            if (!value.empty())
            {
                resultText += anyToString(parseReference(value));
            }

            if (!remainderText.empty())
            {
                resultText += parseText(remainderText);
            }

            return resultText;
        }

        std::vector<Reference> getItems(const pugi::xml_node& inNode)
        {
            if (!Application::hasView())
            {
                return EMPTY_ITEMS;
            }

            View* view = Application::getView();

            std::string value = getAttribute(ITEMS_ATTRIBUTE_NAME, inNode).as_string();

            if (value.empty() || !view->hasVariable(value))
            {
                return EMPTY_ITEMS;
            }

            Reference items = *view->getVariable(value);

            if (!items.isType<std::vector<Reference>>())
            {
                return EMPTY_ITEMS;
            }

            return *items.getValue<std::vector<Reference>>();
        }

        Component::Function getItemGetter(const pugi::xml_node& inNode)
        {
            if (!Application::hasView())
            {
                return nullptr;
            }

            View* view = Application::getView();

            std::string itemGetterFunctionRef = getAttribute(ITEM_GETTER_ATTRIBUTE_NAME, inNode).as_string();

            if (itemGetterFunctionRef.empty() || !view->hasFunction(itemGetterFunctionRef))
            {
                return nullptr;
            }

            return view->getFunction(itemGetterFunctionRef);
        }
    }
}