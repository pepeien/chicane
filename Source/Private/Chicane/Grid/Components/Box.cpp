#include "Chicane/Grid/Components/Box.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace BoxComponent
        {
            Props getProps(const pugi::xml_node& inNode)
            {
                Props result {};
                result.id    = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.style = getStyle(inNode);

                return result;
            }

            void compile(const pugi::xml_node& inNode)
            {
                ImGuiContext& context = *GImGui;
                ImGuiWindow* window   = ImGui::GetCurrentWindow();
    
                if (window->SkipItems)
                {
                    return;
                }

                Props props = getProps(inNode);
                Style style = props.style;
    
                ImVec2 size = ImGui::CalcItemSize(
                    ImVec2(style.width, style.height),
                    ImGui::CalcItemWidth(),
                    context.FontSize + context.Style.FramePadding.y * 2.0f
                );
                ImVec2 position = window->DC.CursorPos;

                ImRect borderBox = ImRect(
                    position.x,
                    position.y,
                    position.x + size.x,
                    position.y + size.y
                );
    
                ImGui::ItemSize(size);
    
                if (!ImGui::ItemAdd(borderBox, 0))
                {
                    return;
                }
    
                ImGui::PushStyleColor(
                    ImGuiCol_FrameBg,
                    hexToColor(style.backgroundColor)
                );
                ImGui::RenderFrame(
                    borderBox.Min,
                    borderBox.Max,
                    ImGui::GetColorU32(ImGuiCol_FrameBg),
                    false,
                    context.Style.FrameRounding
                );
                ImGui::PopStyleColor();
            }
        }
    }
}