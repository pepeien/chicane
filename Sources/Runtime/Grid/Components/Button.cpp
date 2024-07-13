#include "Runtime/Grid/Components/Button.hpp"

#include "Runtime/Core.hpp"
#include "Runtime/Grid/View.hpp"

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

                if (ImGui::Button(processText(text).c_str()))
                {
                    View* view = getActiveView();

                    if (!view)
                    {
                        return;
                    }

                    ComponentFunction onClickFunction = view->getFunction(
                        getAttribute(ON_CLICK_ATTRIBUTE_NAME, outNode).as_string()
                    );

                    if (!onClickFunction)
                    {
                        return;
                    }

                    ComponentEvent event = {};
                    event.values.push_back(outNode);

                    onClickFunction(event);
                }
            }
        }
    }
}