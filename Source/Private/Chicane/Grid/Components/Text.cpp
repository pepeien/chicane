#include "Chicane/Grid/Components/Text.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextComponent
        {
            void compileRaw(const std::string& inText, const Style& inStyle)
            {
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

                std::string rawText = inNode.child_value();

                compileRaw(rawText, getStyle(inNode));
            }
        }
    }
}