#include "Chicane/Grid/Components/List.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ListComponent
        {
            Direction getDirection(const pugi::xml_node& inNode)
            {
                std::string rawDirection = getAttribute(DIRECTION_ATTRIBUTE_NAME, inNode).as_string();

                std::transform(
                    rawDirection.begin(),
                    rawDirection.end(),
                    rawDirection.begin(),
                    ::toupper
                );

                if (rawDirection.compare(DIRECTION_ROW) == 0)
                {
                    return Direction::Row;
                }

                return Direction::Column;
            }

            Props getProps(const pugi::xml_node& inNode)
            {
                Props result {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.direction  = getDirection(inNode);
                result.style      = getStyle(inNode);
                result.items      = getItems(inNode);
                result.itemGetter = getItemGetter(inNode);
                result.children   = inNode.children();

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

                ImGui::PushStyleColor(
                    ImGuiCol_ChildBg,
                    hexToColor(inProps.style.backgroundColor)
                );

                ImGui::BeginChild(
                    inProps.id.c_str(),
                    ImVec2(inProps.style.width, inProps.style.height),
                    ImGuiChildFlags_AlwaysUseWindowPadding
                );
                    for (std::any item : inProps.items)
                    {
                        ComponentEvent event {};
                        event.values.push_back(item);

                        inProps.itemGetter(event);
                    }

                    if (!inProps.children.empty())
                    {
                        for (const pugi::xml_node& child : inProps.children)
                        {
                            if (inProps.direction == Direction::Row)
                            {
                                ImGui::SameLine();
                            }

                            compileChild(child);
                        }
                    }
                ImGui::EndChild();

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