#pragma once

#include "Runtime/Grid/Essential.hpp"
#include "Runtime/Grid/Components/Box.hpp"
#include "Runtime/Grid/Components/Button.hpp"
#include "Runtime/Grid/Components/List.hpp"
#include "Runtime/Grid/Components/ProgressBar.hpp"
#include "Runtime/Grid/Components/Text.hpp"
#include "Runtime/Grid/Components/TextInput.hpp"

namespace Chicane
{
    namespace Grid
    {
        const std::unordered_map<std::string, std::function<void (pugi::xml_node&)>> Components = {
            { BoxComponent::TAG_ID, &BoxComponent::compile },
            { ButtonComponent::TAG_ID, &ButtonComponent::compile },
            { ListComponent::TAG_ID, &ListComponent::compile },
            { ProgressBarComponent::TAG_ID, &ProgressBarComponent::compile },
            { TextComponent::TAG_ID, &TextComponent::compile },
            { TextInputComponent::TAG_ID, &TextInputComponent::compile }
        };
    }
}