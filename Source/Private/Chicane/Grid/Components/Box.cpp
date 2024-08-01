#include "Chicane/Grid/Components/Box.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace BoxComponent
        {
            void compile(const pugi::xml_node& inNode)
            {
                ImGuiContext& context = *GImGui;
                ImGuiWindow* window   = ImGui::GetCurrentWindow();
    
                if (window->SkipItems)
                {
                    return;
                }

                Style gridStyle = getStyle(inNode);
    
                ImGuiStyle& style = context.Style;
                ImVec2 size = ImGui::CalcItemSize(
                    ImVec2(gridStyle.width, gridStyle.height),
                    ImGui::CalcItemWidth(),
                    context.FontSize + style.FramePadding.y * 2.0f
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
    
                ImGui::RenderFrame(
                    borderBox.Min,
                    borderBox.Max,
                    ImGui::GetColorU32(ImGuiCol_FrameBg),
                    false,
                    style.FrameRounding
                );
            }
        }
    }
}