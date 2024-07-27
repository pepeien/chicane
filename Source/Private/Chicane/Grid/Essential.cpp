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
            return std::string(inTarget.end() - inEnding.size(), inTarget.end()).compare(inEnding) == 0;
        }

        std::string getTag(const pugi::xml_node&inNode)
        {
            return inNode.name();
        }

        float getSizeFromPixel(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            std::string rawValue = inAttribute.as_string();

            if (!endsWith(rawValue, "px"))
            {
                return 0.0f;
            }
        
            return std::stof(std::string(rawValue.begin(), rawValue.end() - 2));
        }

        float calculateSizeFromViewportHeight(float inVhValue)
        {
            return State::getResolution().y * (inVhValue / 100);
        }

        float getSizeFromViewportHeight(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            std::string rawValue = inAttribute.as_string();

            if (!endsWith(rawValue, "vh"))
            {
                return 0.0f;
            }

            return calculateSizeFromViewportHeight(
                std::stof(std::string(rawValue.begin(), rawValue.end() - 2))
            );
        }

        float calculateSizeFromViewportWidth(float inVwValue)
        {
            return State::getResolution().x * (inVwValue / 100);
        }

        float getSizeFromViewportWidth(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            std::string rawValue = inAttribute.as_string();

            if (!endsWith(rawValue, "vw"))
            {
                return 0.0f;
            }

            return calculateSizeFromViewportWidth(
                std::stof(std::string(rawValue.begin(), rawValue.end() - 2))
            );
        }

        float getSize(
            const std::string& inAttributeName,
            const pugi::xml_node&inNode
        )
        {
            pugi::xml_attribute attributeValue = getAttribute(inAttributeName, inNode);

            if (attributeValue.empty())
            {
                return 0.0f;
            }

            std::string rawValue = attributeValue.as_string();

            if (endsWith(rawValue, "px"))
            {
                return getSizeFromPixel(attributeValue);
            }

            if (endsWith(rawValue, "vh"))
            {
                return getSizeFromViewportHeight(attributeValue);
            }

            if (endsWith(rawValue, "vw"))
            {
                return getSizeFromViewportWidth(attributeValue);
            }

            return 0.0f;
        }

        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node&inNode
        )
        {
            return inNode.attribute(inName.c_str());
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

        void execOnTick(const pugi::xml_node&inNode)
        {
            std::string onTickFunction = getAttribute(ON_TICK_ATTRIBUTE, inNode).as_string();

            if (onTickFunction.empty())
            {
                return;
            }

            processRefValue(onTickFunction);
        }

        void compileChildren(const pugi::xml_node& outNode)
        {
            for (const pugi::xml_node& child : outNode.children())
            {
                compileChild(child);
            }
        }

        void compileChild(const pugi::xml_node& outNode)
        {
            if (outNode.name() == nullptr)
            {
                return;
            }

            std::string tagName = std::string(outNode.name());

            if (!hasComponent(tagName))
            {
                return;
            }

            execOnTick(outNode);

            getComponent(tagName)(outNode);
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
                return "";
            }

            if (!refValueContainsFunction(inRefValue))
            {
                if (!view->hasVariable(inRefValue))
                {
                    return "";
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