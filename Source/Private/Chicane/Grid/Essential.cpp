#include "Chicane/Grid/Essential.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Grid/Components.hpp"
#include "Chicane/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        std::unordered_map<std::string, View*> m_views;
        View* m_activeView = nullptr;

        bool endsWith(const std::string& inTarget, const std::string& inEnding)
        {
            if (inTarget.size() < inEnding.size())
            {
                return false;
            }

            return std::string(inTarget.end() - inEnding.size(), inTarget.end()).compare(inEnding) == 0;
        }

        std::string getTag(const pugi::xml_node& inNode)
        {
            return inNode.name();
        }

        float getSizeFromPixel(const std::string& inValue)
        {
            if (!endsWith(inValue, "px"))
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

        float calculateSizeFromViewportHeight(float inVhValue)
        {
            return getResolution().y * (inVhValue / 100);
        }

        float getSizeFromViewportHeight(const std::string& inValue)
        {
            if (!endsWith(inValue, "vh"))
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
            return getResolution().x * (inVwValue / 100);
        }

        float getSizeFromViewportWidth(const std::string& inValue)
        {
            if (!endsWith(inValue, "vw"))
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

        float getSize(const std::string inValue)
        {
            if (inValue.empty())
            {
                return 0.0f;
            }

            if (endsWith(inValue, "px"))
            {
                return getSizeFromPixel(inValue);
            }

            if (endsWith(inValue, "vh"))
            {
                return getSizeFromViewportHeight(inValue);
            }

            if (endsWith(inValue, "vw"))
            {
                return getSizeFromViewportWidth(inValue);
            }

            std::string rawValue = anyToString(processRefValue(inValue));

            if (rawValue.empty())
            {
                return 0.0f;
            }

            return std::stof(rawValue);
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

        ComponentMargin getMargin(const pugi::xml_node& inNode)
        {
            ComponentMargin result{};

            pugi::xml_attribute oneline = getAttribute(MARGIN_ATTRIBUTE_NAME, inNode);

            if (!oneline.empty())
            {
                std::vector<std::string> splittedOneline = Utils::split(
                    Utils::trim(oneline.as_string()),
                    MARGIN_SEPARATOR
                );

                if (splittedOneline.size() == 1) // `SINGLE_MARGIN`
                {
                    float margin = getSize(splittedOneline.at(0));

                    result.top    = margin;
                    result.right  = margin;
                    result.bottom = margin;
                    result.left   = margin;
                }

                if (splittedOneline.size() == 2) // `VERTICAL_MARGIN` `HORIZONTAL_MARGIN`
                {
                    float verticalMargin   = getSize(splittedOneline.at(0));
                    float horizontalMargin = getSize(splittedOneline.at(1));

                    result.top    = verticalMargin;
                    result.bottom = verticalMargin;
                    result.right  = horizontalMargin;
                    result.left   = horizontalMargin;
                }

                if (splittedOneline.size() == 3) // `TOP_MARGIN` `BOTTOM_MARGIN` `HORIZONTAL_MARGIN`
                {
                    float topMargin        = getSize(splittedOneline.at(0));
                    float bottomMargin     = getSize(splittedOneline.at(1));
                    float horizontalMargin = getSize(splittedOneline.at(2));

                    result.top    = topMargin;
                    result.bottom = bottomMargin;
                    result.right  = horizontalMargin;
                    result.left   = horizontalMargin;
                }

                if (splittedOneline.size() >= 4) // `TOP_MARGIN` `RIGHT_MARGIN` `BOTTOM_MARGIN` `LEFT_MARGIN`
                {
                    result.top    = getSize(splittedOneline.at(0));
                    result.right  = getSize(splittedOneline.at(1));
                    result.bottom = getSize(splittedOneline.at(2));
                    result.left   = getSize(splittedOneline.at(3));
                }

                return result;
            }

            result.top    = getSize(MARGIN_TOP_ATTRIBUTE_NAME,    inNode);
            result.bottom = getSize(MARGIN_BOTTOM_ATTRIBUTE_NAME, inNode);
            result.left   = getSize(MARGIN_LEFT_ATTRIBUTE_NAME,   inNode);
            result.right  = getSize(MARGIN_RIGHT_ATTRIBUTE_NAME,  inNode);

            return result;
        }

        bool hasViews()
        {
            return m_views.size() > 0;
        }

        void addView(View* inView)
        {
            if (m_views.find(inView->getId()) != m_views.end())
            {
                return;
            }

            m_views.insert(
                std::make_pair(
                    inView->getId(),
                    inView
                )
            );
        }

        void addView(const std::vector<View*>& inViews)
        {
            for (View* view : inViews)
            {
                addView(view);
            }
        }

        View* getActiveView()
        {
            return m_activeView;
        }

        void setActiveView(const std::string& inViewID)
        {
            m_activeView = m_views.at(inViewID);
        }

        void execOnTick(const pugi::xml_node& inNode)
        {
            std::string onTickFunction = getAttribute(ON_TICK_ATTRIBUTE, inNode).as_string();

            if (onTickFunction.empty())
            {
                return;
            }

            processRefValue(onTickFunction);
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
            if (inNode.name() == nullptr)
            {
                return;
            }

            std::string tagName = std::string(inNode.name());

            if (!hasComponent(tagName))
            {
                return;
            }

            execOnTick(inNode);

            ComponentMargin margin = getMargin(inNode);

            ImVec2 position = ImGui::GetCursorPos();
            position.x += margin.left + margin.right;
            position.y += margin.top + margin.bottom;

            ImGui::SetCursorPos(position);

            getComponent(tagName)(inNode);
        }

        std::string anyToString(const std::any& inValue)
        {
            if (inValue.type() == typeid(std::string))
            {
                return std::any_cast<std::string>(inValue);
            }

            if (inValue.type() == typeid(int))
            {
                return std::to_string(std::any_cast<int>(inValue));
            }

            if (inValue.type() == typeid(std::uint64_t))
            {
                return std::to_string(std::any_cast<std::uint64_t>(inValue));
            }

            if (inValue.type() == typeid(std::uint32_t))
            {
                return std::to_string(std::any_cast<std::uint32_t>(inValue));
            }

            if (inValue.type() == typeid(float))
            {
                return std::to_string(std::any_cast<float>(inValue));
            }

            return "";
        }

        ComponentFunctionData parseFunction(const std::string& inRefValue)
        {
            std::string trimmedValue = Utils::trim(inRefValue);

            if (trimmedValue.empty())
            {
                return {};
            }

            std::size_t paramsStart  = trimmedValue.find_first_of(FUNCTION_PARAMS_OPENING);
            std::size_t paramsEnd    = trimmedValue.find_last_of(FUNCTION_PARAMS_CLOSING);

            std::string name   = trimmedValue.substr(0, paramsStart);

            paramsStart += 1;
            paramsEnd   -= paramsStart;

            std::string params = Utils::trim(
                inRefValue.substr(
                    paramsStart,
                    paramsEnd
                )
            );

            ComponentFunctionData data {};
            data.name = name;

            for (std::string& value : Utils::split(params, ','))
            {
                data.params.push_back(
                    processRefValue(Utils::trim(value))
                );
            }

            return data;
        }

        bool textContainsRefValue(const std::string& inText)
        {
            bool hasOpening = inText.find_first_of(REF_VALUE_OPENING) != std::string::npos;
            bool hasClosing = inText.find_first_of(REF_VALUE_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        bool refValueContainsFunction(const std::string& inRefValue)
        {
            bool hasOpening = inRefValue.find_first_of(FUNCTION_PARAMS_OPENING) != std::string::npos;
            bool hasClosing = inRefValue.find_first_of(FUNCTION_PARAMS_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        std::any processRefValue(const std::string& inRefValue)
        {
            View* view = m_activeView;

            if (!view)
            {
                return inRefValue;
            }

            if (!refValueContainsFunction(inRefValue))
            {
                if (!view->hasVariable(inRefValue))
                {
                    return inRefValue;
                }

                return *view->getVariable(inRefValue);
            }

            ComponentFunctionData functionData = parseFunction(inRefValue);
            ComponentFunction functionRef      = view->getFunction(functionData.name);

            if (!functionRef)
            {
                return inRefValue;
            }

            ComponentEvent event {};
            event.values = functionData.params;

            return functionRef(event);
        }

        std::string processText(const std::string& inText)
        {
            if (Utils::trim(inText).empty())
            {
                return " ";
            }

            if (!textContainsRefValue(inText))
            {
                return inText;
            }

            std::size_t foundOpening = inText.find_first_of(REF_VALUE_OPENING);
            std::size_t foundClosing = inText.find_first_of(REF_VALUE_CLOSING);
            std::string resultText = inText.substr(
                0,
                foundOpening
            );
            std::string remainderText = inText.substr(
                foundClosing + 2,
                inText.size() - foundClosing
            );

            foundOpening += 2;
            foundClosing -= foundOpening;

            std::string refValue = inText.substr(
                foundOpening,
                foundClosing
            );
            refValue = Utils::trim(refValue);

            if (!refValue.empty())
            {
                resultText += anyToString(processRefValue(refValue));
            }

            if (!remainderText.empty())
            {
                resultText += processText(remainderText);
            }

            return resultText;
        }
    }
}