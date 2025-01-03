#include "Chicane/Grid/Component/TextInputComponent.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextInputComponent
        {
            void setOnInputData(Props& outProps, const pugi::xml_node& inNode)
            {
                View* view = getActiveView();

                if (!view)
                {
                    return;
                }

                Component::FunctionData functionData = parseFunction(
                    getAttribute(ON_INPUT_ATTRIBUTE_NAME, inNode).as_string()
                );

                if (!view->hasFunction(functionData.name))
                {
                    return;
                }

                outProps.onInput = view->getFunction(
                    functionData.name
                );

                Component::Event event {};
                event.values = functionData.params;

                if (functionData.params.size() == 1)
                {
                    std::any param = functionData.params[0];

                    if (param.type() == typeid(std::string))
                    {
                        if (Utils::trim(std::any_cast<std::string>(param)) == ON_INPUT_EVENT_KEYWORD)
                        {
                            event.values[0] = inNode;
                        }
                    }
                }

                outProps.onInputEvent = event;
            }

            Props getProps(const pugi::xml_node& inNode)
            {
                const std::string& isVisible = processText(getAttribute(IS_VISIBLE_ATTRIBUTE_NAME, inNode).as_string());
    
                Props result {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.bIsVisible = isVisible.empty() || Utils::areEquals(isVisible, "true");
                result.label      = getAttribute(LABEL_ATTRIBUTE_NAME, inNode).as_string();
                result.value      = getActiveView()->getVariable(getAttribute(VALUE_ATTRIBUTE_NAME, inNode).as_string());
                result.style      = Style::getStyle(inNode);
                result.children   = inNode.children();

                setOnInputData(result, inNode);

                return result;
            }

            void validate(const pugi::xml_node& inNode)
            {
                if (!Utils::areEquals(TAG_ID, inNode.name()))
                {
                    throw std::runtime_error("Component is not a " + TAG_ID);
                }

                std::string variable = getAttribute(VALUE_ATTRIBUTE_NAME, inNode).as_string();

                if (variable.empty())
                {
                    throw std::runtime_error(TAG_ID + " components must have a " + VALUE_ATTRIBUTE_NAME + " attribute");
                }

                View* view = Grid::getActiveView();

                if (view == nullptr)
                {
                    throw std::runtime_error("There is no active view");
                }

                if (!view->hasVariable(variable))
                {
                    throw std::runtime_error("The view " + view->getId() + " doesn't have the variable " + variable + " exposed");
                }

                if (view->getVariable(variable)->type() != typeid(std::string))
                {
                    throw std::runtime_error(TAG_ID + " values must be strings");
                }
            }

            void compile(const pugi::xml_node& inNode)
            {
                validate(inNode);

                Props props = getProps(inNode);

                if (!props.bIsVisible)
                {
                    return;
                }
    
                const Style& style = props.style;

                ImGui::PushItemWidth(style.width);
                    if (
                        ImGui::InputText(
                            props.label.empty() ? " " : props.label.c_str(),
                            std::any_cast<std::string>(props.value)
                        )
                    )
                    {
                        if (props.onInput) {
                            props.onInput(props.onInputEvent);
                        }
                    }
                ImGui::PopItemWidth();
            }
        }
    }
}