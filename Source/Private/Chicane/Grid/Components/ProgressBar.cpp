#include "Chicane/Grid/Components/ProgressBar.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ProgressBarComponent
        {
            void compile(const pugi::xml_node& inNode)
            {
                float percentage = std::clamp(
                    getAttribute(PERCENTAGE_ATTRIBUTE_NAME, inNode).as_float(),
                    MIN_PERCENTAGE,
                    MAX_PERCENTAGE
                ) / 100;
    
                ImGui::ProgressBar(
                    percentage,
                    ImVec2(
                        getSize(WIDTH_ATTRIBUTE_NAME, inNode),
                        getSize(HEIGHT_ATTRIBUTE_NAME, inNode)
                    ),
                    inNode.child_value()
                );
            }
        }
    }
}