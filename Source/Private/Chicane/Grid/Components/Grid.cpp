#include "Chicane/Grid/Components/Grid.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Components/List.hpp"
#include "Chicane/Grid/Components/Text.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace GridComponent
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
                Props result {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.itemSize   = getItemSize(inNode);
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

            void showGrid(
                const Props& inProps,
                std::uint32_t inItemCount,
                std::function<void (std::uint32_t)> inItemShowCallback
            )
            {
                if (inProps.items.empty())
                {
                    return;
                }

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

                std::uint32_t columnCount = std::min(
                    inItemCount,
                    static_cast<std::uint32_t>(inProps.style.width / itemSize.x)
                );
                std::uint32_t rowCount = static_cast<std::uint32_t>(
                    std::ceil((float)inItemCount / (float)columnCount)
                );

                std::uint32_t chidrenVisited = 0;

                ImGui::PushStyleColor(
                    ImGuiCol_ChildBg,
                    hexToColor(inProps.style.backgroundColor)
                );
                ImGui::BeginChild(
                    inProps.id.c_str(),
                    wrapperSize,
                    ImGuiChildFlags_AlwaysUseWindowPadding
                );
                    for (std::uint32_t row = 0; row < rowCount; row++)
                    {
                        ImGui::NewLine();

                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() -13);

                        std::string rowId = inProps.id + "Row-" + std::to_string(row);

                        ImGui::BeginChild(
                            rowId.c_str(),
                            rowSize,
                            ImGuiChildFlags_AlwaysUseWindowPadding
                        );
                            for (std::uint32_t column = 0; column < columnCount && chidrenVisited < inItemCount; column++, chidrenVisited++)
                            {
                                ImGui::SameLine();

                                std::string itemId = inProps.id + "Item-" + std::to_string(chidrenVisited);

                                ImGui::BeginChild(
                                    itemId.c_str(),
                                    itemSize,
                                    ImGuiChildFlags_AlwaysUseWindowPadding
                                );
                                    inItemShowCallback(chidrenVisited);
                                ImGui::EndChild();
                            }
                        ImGui::EndChild();
                    }
                ImGui::EndChild();
                ImGui::PopStyleColor();
            }

            void showItems(const Props& inProps)
            {
                if (inProps.items.empty())
                {
                    return;
                }

                showGrid(
                    inProps,
                    static_cast<std::uint32_t>(inProps.items.size()),
                    [inProps](std::uint32_t currentItemIndex) {
                        ComponentEvent event {};
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
                
                showGrid(
                    inProps,
                    static_cast<std::uint32_t>(items.size()),
                    [items](std::uint32_t inCurrentItemIndex) {
                        compileChild(items.at(inCurrentItemIndex));
                    }
                );
            }

            void compileRaw(const Props& inProps)
            {
                validate(inProps);

                showItems(inProps);
                showChildren(inProps);
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