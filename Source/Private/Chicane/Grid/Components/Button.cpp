#include "Chicane/Grid/Components/Button.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Components/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ButtonComponent
        {
            void setOnClickData(Props& outProps, const pugi::xml_node& inNode)
            {
                View* view = getActiveView();

                if (!view)
                {
                    return;
                }

                ComponentFunctionData onClickFunctionData = parseFunction(
                    getAttribute(ON_CLICK_ATTRIBUTE_NAME, inNode).as_string()
                );

                if (!view->hasFunction(onClickFunctionData.name))
                {
                    return;
                }

                outProps.onClick = view->getFunction(
                    onClickFunctionData.name
                );

                ComponentEvent onClickEvent {};
                onClickEvent.values = onClickFunctionData.params;

                if (onClickFunctionData.params.size() == 1)
                {
                    std::any param = onClickFunctionData.params[0];

                    if (param.type() == typeid(std::string))
                    {
                        if (Utils::trim(std::any_cast<std::string>(param)) == "$event")
                        {
                            onClickEvent.values[0] = inNode;
                        }
                    }
                }

                outProps.onClickEvent = onClickEvent;
            }

            Props getProps(const pugi::xml_node& inNode)
            {
                Props result {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.style      = getStyle(inNode);
                result.children   = inNode.children();

                setOnClickData(result, inNode);

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

                ImVec2 initialPosition = ImGui::GetCursorPos();

                if (
                    ImGui::InvisibleButton(
                        inProps.id.c_str(),
                        ImVec2(
                            inProps.style.width,
                            inProps.style.height
                        ),
                        ImGuiButtonFlags_FlattenChildren
                    )
                )
                {
                    if (inProps.onClick)
                    {
                        inProps.onClick(inProps.onClickEvent);
                    }
                }

                ImGui::SetCursorPos(initialPosition);

                ContainerComponent::Props listProps {};
                listProps.id              = inProps.id + "_content";
                listProps.style           = inProps.style;
                listProps.children        = inProps.children;
                listProps._renderers = inProps._renderers;

                ContainerComponent::compileRaw(listProps);
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