#include "Chicane/Grid/Components/Popup.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace PopupComponent
        {
            Props getProps(const pugi::xml_node& inNode)
            {
                const std::string& isVisible = processText(getAttribute(IS_VISIBLE_ATTRIBUTE_NAME, inNode).as_string());
                const std::string& isConstrained = processText(getAttribute(IS_CONSTRAINED_ATTRIBUTE_NAME, inNode).as_string());
                const std::string& isResizable   = processText(getAttribute(IS_RESIZABLE_ATTRIBUTE_NAME, inNode).as_string());
    
                Props result {};
                result.id             = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.bIsVisible     = isVisible.empty() || Utils::areEquals(isVisible, "true");
                result.bIsConstrained = isConstrained.empty() || Utils::areEquals(isConstrained, "true");
                result.bIsResizable   = isResizable.empty() || Utils::areEquals(isResizable, "true");
                result.style          = getStyle(inNode);
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
                if (!Utils::areEquals(TAG_ID, inNode.name()))
                {
                    throw std::runtime_error("Component is not a " + TAG_ID);
                }

                compileRaw(getProps(inNode));
            }
        }
    }
}