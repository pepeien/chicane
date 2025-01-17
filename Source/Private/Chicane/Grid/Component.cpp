#include "Chicane/Grid/Component.hpp"

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component/ButtonComponent.hpp"
#include "Chicane/Grid/Component/GridComponent.hpp"
#include "Chicane/Grid/Component/ContainerComponent.hpp"
#include "Chicane/Grid/Component/PopupComponent.hpp"
#include "Chicane/Grid/Component/ProgressBarComponent.hpp"
#include "Chicane/Grid/Component/TextComponent.hpp"
#include "Chicane/Grid/Component/TextInputComponent.hpp"

static const Chicane::Grid::Component::FunctionData EMPTY_FUNCTION_DATA {};

namespace Chicane
{
    namespace Grid
    {
        std::unordered_map<std::string, Component::Compiler::Function> m_components = {
            { ButtonComponent::TAG_ID, &ButtonComponent::compile },
            { GridComponent::TAG_ID, &GridComponent::compile },
            { ContainerComponent::TAG_ID, &ContainerComponent::compile },
            { PopupComponent::TAG_ID, &PopupComponent::compile },
            { ProgressBarComponent::TAG_ID, &ProgressBarComponent::compile },
            { TextComponent::TAG_ID, &TextComponent::compile },
            { TextInputComponent::TAG_ID, &TextInputComponent::compile }
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