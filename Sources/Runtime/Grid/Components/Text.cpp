#include "Runtime/Grid/Components/Text.hpp"

#include "Runtime/Core.hpp"
#include "Runtime/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextComponent
        {
            void compile(pugi::xml_node& outNode)
            {
                if (std::string(outNode.name()).compare(TAG_ID) != 0)
                {
                    return;
                }
    
                if (outNode.children().empty())
                {
                    return;
                }

                std::string rawText = outNode.child_value();

                ImGui::Text(parseText(rawText).c_str());
            }

            std::string parseText(const std::string& inText)
            {
                std::size_t foundOpening = inText.find_first_of("{{");
                std::size_t foundClosing = inText.find_first_of("}}");

                if (foundOpening == std::string::npos || foundClosing == std::string::npos)
                {
                    return inText;
                }

                std::string resultText    = inText.substr(0, foundOpening);
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

                bool isFunction = refValue.find_first_of('(') != std::string::npos && refValue.find_first_of(')') != std::string::npos;

                if (isFunction)
                {
                    std::size_t functionParamsStart = refValue.find_first_of('(');
                    std::size_t functionParamsEnd   = refValue.find_last_of(')');
                    std::string functionName        = refValue.substr(
                        0,
                        functionParamsStart
                    );

                    Grid::View* view                     = Grid::getActiveView();
                    Grid::ComponentFunction viewFunction = view->getFunction(functionName);

                    isFunction = !!viewFunction;

                    if (isFunction)
                    {
                        functionParamsStart += 1;
                        functionParamsEnd   -= functionParamsStart;

                        std::string functionParams = refValue.substr(
                            functionParamsStart,
                            functionParamsEnd
                        );
                        functionParams = Utils::trim(functionParams);

                        ComponentEvent event = {};
                        event.value = Utils::split(functionParams, ',');

                        resultText += std::any_cast<std::string>(viewFunction(event));
                    }
                }
                
                if (!isFunction)
                {
                    resultText += refValue;
                }

                if (remainderText.find_first_of("{{") != std::string::npos && remainderText.find_first_of("}}") != std::string::npos)
                {
                    resultText += parseText(remainderText);
                }

                return resultText;
            }
        }
    }
}