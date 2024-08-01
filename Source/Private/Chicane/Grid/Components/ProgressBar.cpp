#include "Chicane/Grid/Components/ProgressBar.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ProgressBarComponent
        {
            void compile(const pugi::xml_node& inNode)
            {
                Style style = getStyle(inNode);

                float percentage = std::clamp(
                    getAttribute(PERCENTAGE_ATTRIBUTE_NAME, inNode).as_float(),
                    MIN_PERCENTAGE,
                    MAX_PERCENTAGE
                ) / 100;
    
                ImGui::ProgressBar(
                    percentage,
                    ImVec2(style.width, style.height),
                    inNode.child_value()
                );
            }
        }
    }
}