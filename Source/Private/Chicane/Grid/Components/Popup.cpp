#include "Chicane/Grid/Components/Popup.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace PopupComponent
        {
            Props getProps(const pugi::xml_node& inNode)
            {
                const std::string& isResizeable = processText(getAttribute(IS_RESIZEABLE_ATTRIBUTE_NAME, inNode).as_string());

                Props result {};
                result.id           = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.isResizeable = isResizeable.empty() || isResizeable.compare("true") == 0;
                result.style        = getStyle(inNode);
                result.children     = inNode.children();

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

                const char* id = inProps.id.c_str();

                ImGui::SetNextWindowSize(
                    ImVec2(
                        inProps.style.width,
                        inProps.style.height
                    ),
                    ImGuiCond_Appearing
                );

                ImGui::Begin(id);
                    ImGui::GetCurrentWindow()->ConstraintWindow = ImGui::FindWindowByName(
                        getActiveViewId().c_str()
                    );

                    for (const pugi::xml_node& child : inProps.children)
                    {
                        compileChild(child);
                    }
                ImGui::End();
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