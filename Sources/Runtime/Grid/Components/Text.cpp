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
                if (!doesTextHasRefValue(inText))
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

                if (doesRefValueHasFunction(refValue))
                {
                    resultText += std::any_cast<std::string>(processRefValue(refValue));
                }

                if (doesTextHasRefValue(remainderText))
                {
                    resultText += parseText(remainderText);
                }

                return resultText;
            }
        }
    }
}