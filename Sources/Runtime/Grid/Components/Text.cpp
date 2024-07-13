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

                std::string rawText = outNode.child_value();

                ImGui::Text(processText(rawText).c_str());
            }
        }
    }
}