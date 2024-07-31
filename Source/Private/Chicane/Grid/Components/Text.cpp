#include "Chicane/Grid/Components/Text.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextComponent
        {
            void compile(const pugi::xml_node& inNode)
            {
                if (std::string(inNode.name()).compare(TAG_ID) != 0)
                {
                    return;
                }

                std::string rawText = inNode.child_value();

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