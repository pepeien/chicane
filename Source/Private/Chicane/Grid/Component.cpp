#include "Chicane/Grid/Component.hpp"

#include "Chicane/Grid/Essential.hpp"

static const Chicane::Grid::Component::FunctionData EMPTY_FUNCTION_DATA = {};

namespace Chicane
{
    namespace Grid
    {
        std::unordered_map<std::string, Component::Compiler::Function> m_components = {
            { Button::TAG_ID, &Button::compile },
            { List::TAG_ID, &List::compile },
            { Container::TAG_ID, &Container::compile },
            { Popup::TAG_ID, &Popup::compile },
            { ProgressBar::TAG_ID, &ProgressBar::compile },
            { Text::TAG_ID, &Text::compile },
            { TextInput::TAG_ID, &TextInput::compile }
        };

        const Component::FunctionData& Component::FunctionData::empty()
        {
            return EMPTY_FUNCTION_DATA;
        }

        bool hasComponent(const std::string& inId)
        {
            return m_components.find(inId) != m_components.end();
        }

        void addComponent(const std::string& inId, Component::Compiler::Function inCompiler)
        {
            if (hasComponent(inId))
            {
                return;
            }

            m_components.insert(
                std::make_pair(
                    inId,
                    inCompiler
                )
            );
        }

        Component::Compiler::Function getComponent(const std::string& inId)
        {
            if (!hasComponent(inId))
            {
                throw std::runtime_error("The component [" + inId + "] does not exist.");
            }

            return m_components.at(inId);
        }
    }
}