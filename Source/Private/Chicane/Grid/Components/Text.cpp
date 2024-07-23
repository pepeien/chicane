#include "Chicane/Grid/Components/Text.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"

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

                std::string rawText = outNode.child_value();

                compileRaw(rawText);
            }

            void compileRaw(const std::string& inText)
            {
                ImGui::Text(
                    processText(inText).c_str()
                );
            }
        }
    }
}