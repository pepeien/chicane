#include "Grid/Component/Button.hpp"

#include "Core.hpp"
#include "Grid/Essential.hpp"
#include "Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Button
        {
            void setOnClickData(Props& outProps, const pugi::xml_node& inNode)
            {
                if (!hasView())
                {
                    return;
                }

                View* view = getView();

                Component::FunctionData onClickFunctionData = parseFunction(
                    getAttribute(ON_CLICK_ATTRIBUTE_NAME, inNode).as_string()
                );

                if (!view->hasFunction(onClickFunctionData.name))
                {
                    return;
                }

                outProps.onClick = view->getFunction(onClickFunctionData.name);

                Component::Event onClickEvent = {};
                onClickEvent.values = onClickFunctionData.params;

                if (onClickFunctionData.params.size() == 1)
                {
                    Reference param = onClickFunctionData.params[0];

                    if (param.isType<std::string>())
                    {
                        if (String::trim(*param.getValue<std::string>()) == ON_CLICK_EVENT_KEYWORD)
                        {
                            onClickEvent.values[0] = Reference::fromValue<const pugi::xml_node>(&inNode);
                        }
                    }
                }

                outProps.onClickEvent = onClickEvent;
            }

            Props getProps(const pugi::xml_node& inNode)
            {
                const std::string& isVisible = parseText(
                    getAttribute(IS_VISIBLE_ATTRIBUTE_NAME, inNode).as_string()
                );
    
                Props result = {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.bIsVisible = String::toBool(isVisible);
                result.style      = Style::getStyle(inNode);
                result.children   = inNode.children();

                setOnClickData(result, inNode);

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

            void compileRaw(const Props& inProps)
            {
                assertProps(inProps);

                if (!inProps.bIsVisible)
                {
                    return;
                }

/*
                ImVec2 initialPosition = ImGui::GetCursorPos();

                if (
                    ImGui::InvisibleButton(
                        inProps.id.c_str(),
                        ImVec2(
                            std::max(1.0f, inProps.style.width),
                            std::max(1.0f, inProps.style.height)
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
*/
                Container::Props containerProps {};
                containerProps.id         = inProps.id + "_content";
                containerProps.style      = inProps.style;
                containerProps.children   = inProps.children;
                containerProps._renderers = inProps._renderers;

                Container::compileRaw(containerProps);
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