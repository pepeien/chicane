#include "Runtime/Grid/Components/Button.hpp"

#include "Runtime/Grid/Maps.hpp"

#include "Runtime/Core.hpp"

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
    
                View activeView = getActiveView();
    
                std::string callbackSignature = getAttribute(ON_CLICK_ATTRIBUTE_NAME, outNode).as_string();
    
                if (activeView.callbacks.find(callbackSignature) == activeView.callbacks.end())
                {
                    ImGui::Button(buttonText);
    
                    return;
                }
    
                if (ImGui::Button(buttonText))
                {
                    activeView.callbacks.at(callbackSignature)(outNode);
                }
            }
        }
    }
}