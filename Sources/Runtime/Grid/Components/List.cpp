#include "Runtime/Grid/Components/List.hpp"

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

            void validate(const pugi::xml_node& inNode)
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

            void compile(pugi::xml_node& outNode)
            {
                validate(outNode);

                Direction direction = getDirection(outNode);

                ImGui::BeginChild(
                    getAttribute(ID_ATTRIBUTE_NAME, outNode).as_string(),
                    ImVec2(
                        getSize(WIDTH_ATTRIBUTE_NAME, outNode),
                        getSize(HEIGHT_ATTRIBUTE_NAME, outNode)
                    )
                );
                    if (!outNode.children().empty())
                    {
                        for (pugi::xml_node& child : outNode.children())
                        {
                            if (direction == Direction::Row)
                            {
                                ImGui::SameLine();
                            }

                            compileChild(child);
                        }
                    }
                ImGui::EndChild();
            }
        }
    }
}