#include "Chicane/Grid/Component/ProgressBarComponent.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Essential.hpp"

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
                const std::string& isVisible = parseText(getAttribute(IS_VISIBLE_ATTRIBUTE_NAME, inNode).as_string());
    
                Props result {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.bIsVisible = isVisible.empty() || Utils::areEquals(isVisible, "1") || Utils::areEquals(isVisible, "true");
                result.percentage = getAttribute(PERCENTAGE_ATTRIBUTE_NAME, inNode).as_float();
                result.style      = Style::getStyle(inNode);

                return result;
            }

            void compileRaw(const Props& inProps)
            {
                if (!inProps.bIsVisible)
                {
                    return;
                }
    
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
                if (!Utils::areEquals(TAG_ID, inNode.name()))
                {
                    throw std::runtime_error("Component is not a " + TAG_ID);
                }

                compileRaw(getProps(inNode));
            }
        }
    }
}