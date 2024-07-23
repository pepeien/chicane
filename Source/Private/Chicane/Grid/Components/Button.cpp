#include "Chicane/Grid/Components/Button.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Grid/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ButtonComponent
        {
            void compile(pugi::xml_node& outNode)
            {
                if (std::string(outNode.name()).compare(TAG_ID) != 0)
                {
                    return;
                }

                std::string text = outNode.child_value();

                if (
                    ImGui::Button(
                        processText(text).c_str(),
                        ImVec2(
                            getSize(WIDTH_ATTRIBUTE_NAME, outNode),
                            getSize(HEIGHT_ATTRIBUTE_NAME, outNode)
                        )
                    )
                )
                {
                    View* view = getActiveView();

                    if (!view)
                    {
                        return;
                    }

                    ComponentFunctionData onClickFunctionData = parseFunction(
                        getAttribute(ON_CLICK_ATTRIBUTE_NAME, outNode).as_string()
                    );

                    if (!view->hasFunction(onClickFunctionData.name))
                    {
                        return;
                    }

                    ComponentFunction onClickFunction = view->getFunction(
                        onClickFunctionData.name
                    );

                    ComponentEvent onClickEvent = {};
                    onClickEvent.values = onClickFunctionData.params;

                    if (onClickFunctionData.params.size() == 1)
                    {
                        std::any param = onClickFunctionData.params[0];

                        if (param.type() == typeid(std::string))
                        {
                            if (Utils::trim(std::any_cast<std::string>(param)) == "$event")
                            {
                                onClickEvent.values[0] = outNode;
                            }
                        }
                    }

                    onClickFunction(onClickEvent);
                }
            }
        }
    }
}