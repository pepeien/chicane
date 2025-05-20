#include "Chicane/Grid/Loader.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Loader
        {
            static std::unordered_map<std::string, Component::Compiler> m_components = {
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
                const std::string& tag = inNode.name();

                if (m_components.find(tag) == m_components.end())
                {
                    return nullptr;
                }

                return m_components.at(tag)(inNode);
            }

            void registerComponent(const std::string& inTag, Component::Compiler inCompiler)
            {
                if (m_components.find(inTag) != m_components.end())
                {
                    return;
                }

                m_components.insert(
                    std::make_pair(
                        inTag,
                        inCompiler
                    )
                );
            }
        }
    }
}