#include "Chicane/Grid/Components/List.hpp"

#include "Chicane/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ListComponent
        {
            Direction getDirection(const pugi::xml_node&inNode)
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

            std::vector<std::any> getItems(const pugi::xml_node&inNode)
            {
                View* view = getActiveView();

                if (view == nullptr)
                {
                    return {};
                }

                std::string itemsVariableRef = getAttribute(ITEMS_ATTRIBUTE_NAME, inNode).as_string();

                if (itemsVariableRef.empty() || !view->hasVariable(itemsVariableRef))
                {
                    return {};
                }

                std::any items = *view->getVariable(itemsVariableRef);

                if (items.type() != typeid(std::vector<std::any>))
                {
                    return {};
                }

                return std::any_cast<std::vector<std::any>>(items);
            }

            ComponentFunction getItemGetter(const pugi::xml_node&inNode)
            {
                View* view = getActiveView();

                if (view == nullptr)
                {
                    return {};
                }

                std::string itemGetterFunctionRef = getAttribute(ITEM_GETTER_ATTRIBUTE_NAME, inNode).as_string();

                if (itemGetterFunctionRef.empty() || !view->hasFunction(itemGetterFunctionRef))
                {
                    return {};
                }

                return view->getFunction(itemGetterFunctionRef);
            }

            ImVec4 getBackgroundColor(const pugi::xml_node&inNode)
            {
                std::string backgroundColor = getAttribute(BACKGROUND_COLOR_ATTRIBUTE_NAME, inNode).as_string();

                if (backgroundColor.empty() || backgroundColor.size() < 7 || backgroundColor.size() > 9)
                {
                    return {};
                }

                backgroundColor = backgroundColor.substr(
                    1,
                    backgroundColor.size() - 1
                );
                backgroundColor = backgroundColor.size() == 6 ? backgroundColor + "ff" : backgroundColor;
                std::uint32_t color = std::stoul(
                    backgroundColor,
                    nullptr,
                    16
                );

                // IDK why the result is #AABBGGRR A.K.A reversed
                ImVec4 reversedResult = ImGui::ColorConvertU32ToFloat4(color);
                return { reversedResult.w, reversedResult.z, reversedResult.y, reversedResult.x };
            }

            void validate(const pugi::xml_node&inNode)
            {
                if (TAG_ID.compare(inNode.name()) != 0)
                {
                    throw std::runtime_error("Component is not a " + TAG_ID);
                }

                std::string title = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();

                if (title.empty())
                {
                    throw std::runtime_error(TAG_ID + " components must have a " + ID_ATTRIBUTE_NAME + " attribute");
                }
            }

            void compile(const pugi::xml_node& outNode)
            {
                validate(outNode);

                Direction direction          = getDirection(outNode);
                std::vector<std::any> items  = getItems(outNode);
                ComponentFunction itemGetter = getItemGetter(outNode);
                ImVec4 backgroundColor       = getBackgroundColor(outNode);

                ImGui::PushStyleColor(
                    ImGuiCol_ChildBg,
                    backgroundColor
                );

                ImGui::BeginChild(
                    getAttribute(ID_ATTRIBUTE_NAME, outNode).as_string(),
                    ImVec2(
                        getSize(WIDTH_ATTRIBUTE_NAME, outNode),
                        getSize(HEIGHT_ATTRIBUTE_NAME, outNode)
                    ),
                    ImGuiChildFlags_AlwaysUseWindowPadding
                );
                    for (std::any item : items)
                    {
                        ComponentEvent event {};
                        event.values.push_back(item);

                        itemGetter(event);
                    }

                    if (!outNode.children().empty())
                    {
                        for (const pugi::xml_node& child : outNode.children())
                        {
                            if (direction == Direction::Row)
                            {
                                ImGui::SameLine();
                            }

                            compileChild(child);
                        }
                    }
                ImGui::EndChild();

                ImGui::PopStyleColor();
            }
        }
    }
}