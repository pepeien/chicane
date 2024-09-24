#include "Chicane/Grid/Components/Container.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ContainerComponent
        {
            Props getProps(const pugi::xml_node& inNode)
            {
                const std::string& isVisible = processText(getAttribute(IS_VISIBLE_ATTRIBUTE_NAME, inNode).as_string());

                Props result {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.bIsVisible = isVisible.empty() || Utils::areEquals(isVisible, "true");
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

                if (!inProps.bIsVisible)
                {
                    return;
                }

                ImGui::PushStyleColor(
                    ImGuiCol_ChildBg,
                    hexToImGuiColor(inProps.style.backgroundColor)
                );
                    ImGui::BeginChild(
                        inProps.id.c_str(),
                        ImVec2(inProps.style.width, inProps.style.height)
                    );
                        std::uint32_t i = 0;

                        for (const ComponentFunction& renderer : inProps._renderers)
                        {
                            handlePositioning(inProps, i);

                            renderer({});

                            i++;
                        }

                        for (std::any item : inProps.items)
                        {
                            handlePositioning(inProps, i);

                            ComponentEvent event {};
                            event.values.push_back(item);

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