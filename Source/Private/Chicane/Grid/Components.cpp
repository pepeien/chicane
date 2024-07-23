#include "Chicane/Grid/Components.hpp"

#include "Chicane/Grid/Components/Box.hpp"
#include "Chicane/Grid/Components/Button.hpp"
#include "Chicane/Grid/Components/List.hpp"
#include "Chicane/Grid/Components/ProgressBar.hpp"
#include "Chicane/Grid/Components/Text.hpp"
#include "Chicane/Grid/Components/TextInput.hpp"

namespace Chicane
{
    namespace Grid
    {
        std::unordered_map<std::string, ComponentCompiler> m_components = {
            { BoxComponent::TAG_ID, &BoxComponent::compile },
            { ButtonComponent::TAG_ID, &ButtonComponent::compile },
            { ListComponent::TAG_ID, &ListComponent::compile },
            { ProgressBarComponent::TAG_ID, &ProgressBarComponent::compile },
            { TextComponent::TAG_ID, &TextComponent::compile },
            { TextInputComponent::TAG_ID, &TextInputComponent::compile }
        };

        bool hasComponent(const std::string& inId)
        {
            return m_components.find(inId) != m_components.end();
        }

        void addComponent(const std::string& inId, ComponentCompiler inCompiler)
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

        ComponentCompiler getComponent(const std::string& inId)
        {
            if (!hasComponent(inId))
            {
                throw std::runtime_error("The component [" + inId + "] does not exist.");
            }

            return m_components.at(inId);
        }
    }
}