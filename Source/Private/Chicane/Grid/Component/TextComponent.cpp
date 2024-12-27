#include "Chicane/Grid/Component/TextComponent.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextComponent
        {
            void handlePositioning(const std::string& inText, const Style& inStyle)
            {
                if (inStyle.horizontalAlignment == Style::Alignment::Start && inStyle.verticalAlignment == Style::Alignment::Start)
                {
                    return;
                }

                ImVec2 textSize = ImGui::CalcTextSize(inText.c_str());

                if (inStyle.horizontalAlignment == Style::Alignment::Center || inStyle.horizontalAlignment == Style::Alignment::End)
                {
                    float diff = ImGui::GetCursorPosX() + getSize("100%", Style::Direction::Horizontal) - textSize.x;

                    ImGui::SetCursorPosX(diff);

                    if (inStyle.horizontalAlignment == Style::Alignment::Center)
                    {
                        ImGui::SetCursorPosX(diff * 0.5f);
                    }
                }

                if (inStyle.verticalAlignment == Style::Alignment::Center || inStyle.verticalAlignment == Style::Alignment::End)
                {
                    float diff = ImGui::GetCursorPosY() + getSize("100%", Style::Direction::Vertical) - textSize.y;

                    ImGui::SetCursorPosY(diff);

                    if (inStyle.verticalAlignment == Style::Alignment::Center)
                    {
                        ImGui::SetCursorPosY(diff * 0.5f);
                    }
                }
            }

            void compileRaw(const std::string& inText, const Style& inStyle)
            {
                handlePositioning(inText, inStyle);

                ImGui::PushStyleColor(
                    ImGuiCol_Text,
                    hexToImGuiColor(inStyle.foregroundColor)
                );
                    ImGui::TextWrapped(processText(inText).c_str());
                ImGui::PopStyleColor();
            }

            void compile(const pugi::xml_node& inNode)
            {
                if (!Utils::areEquals(TAG_ID, inNode.name()))
                {
                    throw std::runtime_error("Component is not a " + TAG_ID);
                }

                compileRaw(inNode.child_value(), Style::getStyle(inNode));
            }
        }
    }
}