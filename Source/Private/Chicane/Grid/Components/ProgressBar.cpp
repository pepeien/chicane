#include "Chicane/Grid/Components/ProgressBar.hpp"

#include "Chicane/Grid.hpp"

constexpr float MIN_PERCENTAGE = 0.0f;
constexpr float MAX_PERCENTAGE = 100.0f;

namespace Chicane
{
    namespace Grid
    {
        namespace ProgressBarComponent
        {
            Props getProps(const pugi::xml_node& inNode)
            {
                Props result {};
                result.percentage = getAttribute(PERCENTAGE_ATTRIBUTE_NAME, inNode).as_float();
                result.style      = getStyle(inNode);

                return result;
            }

            void compileRaw(const Props& inProps)
            {
                float percentage = std::clamp(
                    inProps.percentage,
                    MIN_PERCENTAGE,
                    MAX_PERCENTAGE
                ) / 100;
    
                ImGui::ProgressBar(
                    percentage,
                    ImVec2(inProps.style.width, inProps.style.height),
                    inProps.text.c_str()
                );
            }

            void compile(const pugi::xml_node& inNode)
            {
                if (TAG_ID.compare(inNode.name()) != 0)
                {
                    throw std::runtime_error("Component is not a " + TAG_ID);
                }

                compileRaw(getProps(inNode));
            }
        }
    }
}