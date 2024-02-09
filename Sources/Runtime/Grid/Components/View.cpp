#include "Runtime/Grid/Components/View.hpp"

#include "Runtime/Grid/Maps.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ViewComponent
        {
            void validate(const pugi::xml_node& inNode)
            {
                bool isRoot  = inNode.parent() == inNode.root();
                bool isAlone = isRoot && inNode.next_sibling() == nullptr;
    
                if (!isRoot || !isAlone)
                {
                    throw std::runtime_error("UI document root element must not have any siblings");
                }
    
                if (TAG_ID.compare(inNode.name()) != 0)
                {
                    throw std::runtime_error("UI document root element must be a " + TAG_ID);
                }
    
                std::string viewID = getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();
    
                if (viewID.empty())
                {
                    return;
                }
    
                if (Views.find(viewID) == Views.end())
                {
                    throw std::runtime_error("View ID " + viewID + " is not registered on the system");
                }
            }
    
            void compile(
                pugi::xml_node& outNode,
                const Chicane::Vec2& inResolution,
                const Chicane::Vec2& inPosition
            )
            {
                validate(outNode);
    
                ImGuiWindowFlags viewFlags = ImGuiWindowFlags_NoNav |
                                             ImGuiWindowFlags_NoDecoration |
                                             ImGuiWindowFlags_NoMove |
                                             ImGuiWindowFlags_NoBackground;
    
                View activeView = {};
                activeView.callbacks = Views.at(getAttribute(ID_ATTRIBUTE_NAME, outNode).as_string());
    
                setActiveView(activeView);
    
                ImGui::Begin(
                    getAttribute(ID_ATTRIBUTE_NAME ,outNode).as_string(),
                    nullptr,
                    viewFlags
                );
                    ImGui::SetWindowSize(
                        ImVec2(
                            static_cast<float>(inResolution.x),
                            static_cast<float>(inResolution.y)
                        )
                    );
                    ImGui::SetWindowPos(
                        ImVec2(
                            static_cast<float>(inPosition.x),
                            static_cast<float>(inPosition.y)
                        )
                    );
    
                    compileChildren(outNode);
                ImGui::End();
            }
        }
    }
}