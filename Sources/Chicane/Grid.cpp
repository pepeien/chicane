#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        static std::unordered_map<String, Component::Compiler> g_components = {
            { Button::TAG_ID,      [](const pugi::xml_node& inNode) { return new Button(inNode); } },
            { List::TAG_ID,        [](const pugi::xml_node& inNode) { return new List(inNode); } },
            { Container::TAG_ID,   [](const pugi::xml_node& inNode) { return new Container(inNode); } },
            { Popup::TAG_ID,       [](const pugi::xml_node& inNode) { return new Popup(inNode); } },
            { ProgressBar::TAG_ID, [](const pugi::xml_node& inNode) { return new ProgressBar(inNode); } },
            { Text::TAG_ID,        [](const pugi::xml_node& inNode) { return new Text(inNode); } },
            { TextInput::TAG_ID,   [](const pugi::xml_node& inNode) { return new TextInput(inNode); } }
        };

        Component* createComponent(const pugi::xml_node& inNode)
        {
            const String& tag = inNode.name();

            if (g_components.find(tag) == g_components.end())
            {
                return nullptr;
            }

            return g_components.at(tag)(inNode);
        }

        void registerComponent(const String& inTag, Component::Compiler inCompiler)
        {
            if (g_components.find(inTag) != g_components.end())
            {
                return;
            }

            g_components.insert(std::make_pair(inTag, inCompiler));
        }
    }
}