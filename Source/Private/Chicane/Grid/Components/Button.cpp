#include "Chicane/Grid/Components/Button.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ButtonComponent
        {
            void compile(const pugi::xml_node& inNode)
            {
                if (std::string(inNode.name()).compare(TAG_ID) != 0)
                {
                    return;
                }

                std::string text = inNode.child_value();

                Style style = getStyle(inNode);

                if (
                    ImGui::Button(
                        processText(text).c_str(),
                        ImVec2(style.width, style.height)
                    )
                )
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

                    ComponentFunction onClickFunction = view->getFunction(
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

                    onClickFunction(onClickEvent);
                }
            }
        }
    }
}