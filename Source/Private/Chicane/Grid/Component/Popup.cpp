#include "Chicane/Grid/Component/Popup.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Popup
        {
            Props getProps(const pugi::xml_node& inNode)
            {
                const std::string& isVisible     = parseText(getAttribute(IS_VISIBLE_ATTRIBUTE_NAME, inNode).as_string());
                const std::string& isConstrained = parseText(getAttribute(IS_CONSTRAINED_ATTRIBUTE_NAME, inNode).as_string());
                const std::string& isResizable   = parseText(getAttribute(IS_RESIZABLE_ATTRIBUTE_NAME, inNode).as_string());

                Props result = {};
                result.id             = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.bIsVisible     = isVisible.empty() || String::areEquals(isVisible, "1") || String::areEquals(isVisible, "true");
                result.bIsConstrained = isConstrained.empty() || String::areEquals(isConstrained, "1") || String::areEquals(isConstrained, "true");
                result.bIsResizable   = isResizable.empty() || String::areEquals(isResizable, "1") || String::areEquals(isResizable, "true");
                result.style          = Style::getStyle(inNode);
                result.children       = inNode.children();

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

                if (!inProps.bIsVisible)
                {
                    return;
                }

                const char* id = inProps.id.c_str();

                ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar;

                if (!inProps.bIsResizable)
                {
                    flags |= ImGuiWindowFlags_NoResize;
                }

                ImGui::SetNextWindowSize(
                    ImVec2(
                        inProps.style.width,
                        inProps.style.height
                    ),
                    ImGuiCond_Appearing
                );

                if (inProps.bIsConstrained)
                {
                    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
                }

                ImGui::PushStyleColor(
                    ImGuiCol_WindowBg,
                    hexToImGuiColor(inProps.style.backgroundColor)
                );
                    ImGui::Begin(id, nullptr, flags);
                        ImGui::GetCurrentWindow()->IsConstrained = inProps.bIsConstrained;

                        for (const pugi::xml_node& child : inProps.children)
                        {
                            compileChild(child);
                        }
                    ImGui::End();
                ImGui::PopStyleColor();
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