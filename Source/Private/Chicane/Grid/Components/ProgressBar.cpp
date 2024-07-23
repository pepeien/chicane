#include "Chicane/Grid/Components/ProgressBar.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ProgressBarComponent
        {
            void compile(const pugi::xml_node& outNode)
            {
                float percentage = std::clamp(
                    getAttribute(PERCENTAGE_ATTRIBUTE_NAME, outNode).as_float(),
                    MIN_PERCENTAGE,
                    MAX_PERCENTAGE
                ) / 100;
    
                ImGui::ProgressBar(
                    percentage,
                    ImVec2(
                        getSize(WIDTH_ATTRIBUTE_NAME, outNode),
                        getSize(HEIGHT_ATTRIBUTE_NAME, outNode)
                    ),
                    outNode.child_value()
                );
            }
        }
    }
}