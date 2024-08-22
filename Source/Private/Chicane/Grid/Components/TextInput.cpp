#include "Chicane/Grid/Components/TextInput.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextInputComponent
        {
            void validate(const pugi::xml_node& inNode)
            {
                if (TAG_ID.compare(inNode.name()) != 0)
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

                std::string label          = getAttribute(LABEL_ATTRIBUTE_NAME, inNode).as_string();
                std::string value          = getAttribute(VALUE_ATTRIBUTE_NAME, inNode).as_string();

                ImGui::InputText(
                    label.empty() ? " " : label.c_str(),
                    std::any_cast<std::string>(
                        Grid::getActiveView()->getVariable(value)
                    )
                );
            }
        }
    }
}