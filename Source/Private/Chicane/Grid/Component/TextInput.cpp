#include "Chicane/Grid/Component/TextInput.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextInput
        {
            void setOnInputData(Props& outProps, const pugi::xml_node& inNode)
            {
                if (!Application::hasView())
                {
                    return;
                }

                View* view = Application::getView();

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
                    const Reference& param = functionData.params[0];

                    if (param.isType<std::string>())
                    {
                        if (Utils::trim(*param.getValue<std::string>()) == ON_INPUT_EVENT_KEYWORD)
                        {
                            event.values[0] = Reference::fromValue<const pugi::xml_node>(&inNode);
                        }
                    }
                }

                outProps.onInputEvent = event;
            }

            Props getProps(const pugi::xml_node& inNode)
            {
                const std::string& isVisible = parseText(getAttribute(IS_VISIBLE_ATTRIBUTE_NAME, inNode).as_string());
    
                Props result {};
                result.id         = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
                result.bIsVisible = isVisible.empty() || Utils::areEquals(isVisible, "1") || Utils::areEquals(isVisible, "true");
                result.label      = getAttribute(LABEL_ATTRIBUTE_NAME, inNode).as_string();
                result.value      = *Application::getView()->getVariable(getAttribute(VALUE_ATTRIBUTE_NAME, inNode).as_string());
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

                if (!Application::hasView())
                {
                    throw std::runtime_error("There is no active view");
                }

                View* view = Application::getView();

                if (!view->hasVariable(variable))
                {
                    throw std::runtime_error("The view " + view->getId() + " doesn't have the variable " + variable + " exposed");
                }

                if (view->getVariable(variable)->isType<std::string>())
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
                    const char* label = props.label.empty() ? " " : props.label.c_str();

                    if (ImGui::InputText(label, nullptr))
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