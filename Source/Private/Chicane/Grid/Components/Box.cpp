#include "Chicane/Grid/Components/Box.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace BoxComponent
        {
            void compile(const pugi::xml_node& outNode)
            {
                ImGuiContext& context = *GImGui;
                ImGuiWindow* window   = ImGui::GetCurrentWindow();
    
                if (window->SkipItems)
                {
                    return;
                }
    
                ImGuiStyle& style = context.Style;
                ImVec2 size = ImGui::CalcItemSize(
                    ImVec2(
                        getSize(WIDTH_ATTRIBUTE_NAME, outNode),
                        getSize(HEIGHT_ATTRIBUTE_NAME, outNode)
                    ),
                    ImGui::CalcItemWidth(),
                    context.FontSize + style.FramePadding.y * 2.0f
                );
                ImVec2 position = window->DC.CursorPos;
                ImRect borderBox = ImRect(position.x, position.y, position.x + size.x, position.y + size.y);
    
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