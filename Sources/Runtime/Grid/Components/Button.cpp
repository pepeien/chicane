#include "Runtime/Grid/Components/Button.hpp"

#include "Runtime/Core.hpp"
#include "Runtime/Grid/Maps.hpp"
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
    
                if (outNode.children().empty())
                {
                    return;
                }
    
                auto buttonText = outNode.child_value();
    
                View* activeView = getActiveView();
    
                std::string callbackSignature = getAttribute(ON_CLICK_ATTRIBUTE_NAME, outNode).as_string();
    
                if (!activeView->hasFunction(callbackSignature))
                {
                    ImGui::Button(buttonText);
    
                    return;
                }
    
                if (ImGui::Button(buttonText))
                {
                    //activeView->execCallback(callbackSignature, outNode);
                }
            }
        }
    }
}