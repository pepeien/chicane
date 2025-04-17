#include "Grid/Component/List.hpp"

#include "Core.hpp"
#include "Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace List
        {
            Vec<2, float> getItemSize(const pugi::xml_node& inNode)
            {
                float width  = getSize(ITEM_WIDTH_ATTRIBUTE_NAME, inNode);
                float height = getSize(ITEM_HEIGHT_ATTRIBUTE_NAME, inNode);

                return Vec<2, float>(
                    width,
                    height
                );
            }

            Props getProps(const pugi::xml_node& inNode)
            {
                const std::string& isVisible = parseText(
                    getAttribute(IS_VISIBLE_ATTRIBUTE_NAME, inNode).as_string()
                );

                Props result = {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.bIsVisible = String::toBool(isVisible);
                result.itemSize   = getItemSize(inNode);
                result.style      = Style::getStyle(inNode);
                result.items      = getItems(inNode);
                result.itemGetter = getItemGetter(inNode);
                result.children   = inNode.children();

                return result;
            }

            void assertProps(const Props& inProps)
            {
                if (inProps.id.empty())
                {
                    throw std::runtime_error(
                        TAG_ID + " components must have a " + ID_ATTRIBUTE_NAME + " attribute"
                    );
                }
            }

            void showList(
                const Props& inProps,
                std::uint32_t inItemCount,
                std::function<void (std::uint32_t)> inItemShowCallback
            )
            {
                if (inProps.items.empty())
                {
                    return;
                }
/*
                ImVec2 wrapperSize(
                    inProps.style.width,
                    inProps.style.height
                );
                ImVec2 itemSize(
                    std::max(1.0f, inProps.itemSize.x),
                    std::max(1.0f, inProps.itemSize.y)
                );
                ImVec2 rowSize(
                    wrapperSize.x,
                    itemSize.y
                );

                float verticalGap   = inProps.style.gap.top / 2.0f + inProps.style.gap.bottom / 2.0f;
                float horizontalGap = inProps.style.gap.left / 2.0f + inProps.style.gap.right / 2.0f;

                std::uint32_t columnCount = std::min(
                    inItemCount,
                    static_cast<std::uint32_t>(inProps.style.width / (itemSize.x + horizontalGap))
                );
                std::uint32_t rowCount = static_cast<std::uint32_t>(
                    std::ceil((float)inItemCount / (float)columnCount)
                );

                std::uint32_t chidrenVisited = 0;

                ImGui::PushStyleColor(
                    ImGuiCol_ChildBg,
                    hexToImGuiColor(inProps.style.backgroundColor)
                );
                    ImGui::BeginChild(
                        inProps.id.c_str(),
                        wrapperSize
                    );
                        for (std::uint32_t row = 0; row < rowCount; row++)
                        {
                            ImGui::NewLine();

                            float nextVerticalPosition = ImGui::GetCursorPosY() - 13;

                            if (row > 0)
                            {
                                nextVerticalPosition += verticalGap;
                            }

                            ImGui::SetCursorPosY(nextVerticalPosition);

                            std::string rowId = inProps.id + "Row-" + std::to_string(row);

                            ImGui::BeginChild(rowId.c_str(), rowSize);
                                for (std::uint32_t column = 0; column < columnCount && chidrenVisited < inItemCount; column++, chidrenVisited++)
                                {
                                    ImGui::SameLine();

                                    float nextHorizontalPosition = ImGui::GetCursorPosX();

                                    if (column > 0)
                                    {
                                        nextHorizontalPosition += horizontalGap;
                                    }

                                    ImGui::SetCursorPosX(nextHorizontalPosition);

                                    std::string itemId = inProps.id + "Item-" + std::to_string(chidrenVisited);

                                    ImGui::BeginChild(itemId.c_str(), itemSize);
                                        inItemShowCallback(chidrenVisited);
                                    ImGui::EndChild();
                                }
                            ImGui::EndChild();
                        }
                    ImGui::EndChild();
                ImGui::PopStyleColor();
*/
            }

            void showRenderers(const Props& inProps)
            {
                if (inProps._renderers.empty())
                {
                    return;
                }

                showList(
                    inProps,
                    static_cast<std::uint32_t>(inProps._renderers.size()),
                    [inProps](std::uint32_t currentItemIndex) {
                        inProps._renderers.at(currentItemIndex)({});
                    }
                );
            }

            void showItems(const Props& inProps)
            {
                if (inProps.items.empty())
                {
                    return;
                }

                showList(
                    inProps,
                    static_cast<std::uint32_t>(inProps.items.size()),
                    [inProps](std::uint32_t currentItemIndex) {
                        Component::Event event = {};
                        event.values.push_back(inProps.items.at(currentItemIndex));

                        inProps.itemGetter(event);
                    }
                );
            }

            void showChildren(const Props& inProps)
            {
                if (inProps.children.empty())
                {
                    return;
                }

                std::vector<pugi::xml_node> items = extractChildren(inProps.children);
                
                showList(
                    inProps,
                    static_cast<std::uint32_t>(items.size()),
                    [items](std::uint32_t inCurrentItemIndex) {
                        compileChild(items.at(inCurrentItemIndex));
                    }
                );
            }

            void compileRaw(const Props& inProps)
            {
                assertProps(inProps);

                if (!inProps.bIsVisible)
                {
                    return;
                }

                showRenderers(inProps);
                showItems(inProps);
                showChildren(inProps);
            }

            void compile(const pugi::xml_node& inNode)
            {
                if (!String::areEquals(TAG_ID, inNode.name()))
                {
                    throw std::runtime_error("Component is not a " + TAG_ID);
                }

                compileRaw(getProps(inNode));
            }
        }
    }
}