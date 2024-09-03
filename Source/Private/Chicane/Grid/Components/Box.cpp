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

            void compileRaw(const Props& inProps)
            {
                Style style = inProps.style;

                ImVec2 size      = ImVec2(style.width, style.height);
                ImVec2 position  = ImGui::GetCursorPos();
                ImRect rect      = ImRect(
                    position.x,
                    position.y,
                    position.x + size.x,
                    position.y + size.y
                );
    
                ImGui::ItemSize(size);
    
                if (!ImGui::ItemAdd(rect, 0))
                {
                    return;
                }
    
                ImGui::PushStyleColor(
                    ImGuiCol_FrameBg,
                    hexToColor(style.backgroundColor)
                );
                ImGui::RenderFrame(
                    rect.Min,
                    rect.Max,
                    ImGui::GetColorU32(ImGuiCol_FrameBg),
                    false
                );
                ImGui::PopStyleColor();
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