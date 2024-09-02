#include "Chicane/Grid/Components/Popup.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace PopupComponent
        {
            Props getProps(const pugi::xml_node& inNode)
            {
                const std::string& isVisible = processText(getAttribute(IS_VISIBLE_ATTRIBUTE_NAME, inNode).as_string());

                Props result {};
                result.id        = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.isVisible = isVisible.empty() || isVisible.compare("true") == 0;
                result.style     = getStyle(inNode);
                result.children  = inNode.children();

                return result;
            }

            void validate(const Props& inProps)
            {
                if (inProps.id.empty())
                {
                    throw std::runtime_error(TAG_ID + " components must have a " + ID_ATTRIBUTE_NAME + " attribute");
                }
            }

            void compileRaw(const Props& inProps)
            {
                validate(inProps);

                if (!inProps.isVisible)
                {
                    return;
                }

                ImGui::SetNextWindowSize(
                    ImVec2(
                        inProps.style.width,
                        inProps.style.height
                    )
                );
                ImGui::PushStyleColor(
                    ImGuiCol_ChildBg,
                    hexToColor(inProps.style.backgroundColor)
                );

                const char* id = inProps.id.c_str();

                ImGui::OpenPopup(id);
                ImGui::BeginPopup(id, ImGuiWindowFlags_AlwaysAutoResize);
                    for (const pugi::xml_node& child : inProps.children)
                    {
                        compileChild(child);
                    }
                ImGui::EndPopup();

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