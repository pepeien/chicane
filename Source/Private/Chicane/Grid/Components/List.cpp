#include "Chicane/Grid/Components/List.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ListComponent
        {
            Props getProps(const pugi::xml_node& inNode)
            {
                Props result {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.style      = getStyle(inNode);
                result.items      = getItems(inNode);
                result.itemGetter = getItemGetter(inNode);
                result.children   = inNode.children();

                return result;
            }

            void handlePositioning(const Props& inProps, std::uint32_t inIndex)
            {
                if (inProps.style.listDirection == ListDirection::Row)
                {
                    ImGui::SameLine();

                    if (inIndex > 0)
                    {
                        ImGui::SetCursorPosX(
                            ImGui::GetCursorPosX() + ((inProps.style.gap.left / 2) + (inProps.style.gap.right / 2))
                        );
                    }

                    return;
                }

                if (inIndex > 0)
                {
                    ImGui::SetCursorPosY(
                        ImGui::GetCursorPosY() + ((inProps.style.gap.top / 2) + (inProps.style.gap.bottom / 2))
                    );
                }
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

                ImGui::PushStyleColor(
                    ImGuiCol_ChildBg,
                    hexToColor(inProps.style.backgroundColor)
                );
                    ImGui::BeginChild(
                        inProps.id.c_str(),
                        ImVec2(inProps.style.width, inProps.style.height),
                        0,
                        ImGuiWindowFlags_NoInputs
                    );
                        std::uint32_t i = 0;

                        for (std::any item : inProps.items)
                        {
                            ComponentEvent event {};
                            event.values.push_back(item);

                            handlePositioning(inProps, i);

                            inProps.itemGetter(event);

                            i++;
                        }

                        for (const pugi::xml_node& child : inProps.children)
                        {
                            handlePositioning(inProps, i);

                            compileChild(child);

                            i++;
                        }
                    ImGui::EndChild();
                ImGui::PopStyleColor();
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