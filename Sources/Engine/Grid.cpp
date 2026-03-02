#include "Chicane/Grid.hpp"

#include "Chicane/Grid/Component/Button.hpp"
#include "Chicane/Grid/Component/Container.hpp"
#include "Chicane/Grid/Component/Popup.hpp"
#include "Chicane/Grid/Component/ProgressBar.hpp"
#include "Chicane/Grid/Component/Text.hpp"
#include "Chicane/Grid/Component/Text/Input.hpp"

namespace Chicane
{
    namespace Grid
    {
        static std::unordered_map<String, Component::Compiler> g_components = {
            {Button::TAG_ID,      [](const pugi::xml_node& inNode) { return new Button(inNode); }     },
            {Container::TAG_ID,   [](const pugi::xml_node& inNode) { return new Container(inNode); }  },
            {Popup::TAG_ID,       [](const pugi::xml_node& inNode) { return new Popup(inNode); }      },
            {ProgressBar::TAG_ID, [](const pugi::xml_node& inNode) { return new ProgressBar(inNode); }},
            {Text::TAG_ID,        [](const pugi::xml_node& inNode) { return new Text(inNode); }       },
            {TextInput::TAG_ID,   [](const pugi::xml_node& inNode) { return new TextInput(inNode); }  }
        };

        Component* createComponent(const pugi::xml_node& inNode)
        {
            const auto& found = g_components.find(inNode.name());

            if (found == g_components.end())
            {
                return nullptr;
            }

            return found->second(inNode);
        }
    }
}