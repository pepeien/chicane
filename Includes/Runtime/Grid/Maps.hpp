#pragma once

#include "Runtime/Grid/Essential.hpp"
#include "Runtime/Grid/Components/Box.hpp"
#include "Runtime/Grid/Components/Button.hpp"
#include "Runtime/Grid/Components/List.hpp"
#include "Runtime/Grid/Components/ProgressBar.hpp"
#include "Runtime/Grid/Components/Text.hpp"
#include "Runtime/Grid/Views/Gameplay.hpp"

namespace Chicane
{
    namespace Grid
    {
        const ComponentCallbackMap Components = {
            { BoxComponent::TAG_ID, &BoxComponent::compile },
            { ButtonComponent::TAG_ID, &ButtonComponent::compile },
            { ListComponent::TAG_ID, &ListComponent::compile },
            { ProgressBarComponent::TAG_ID, &ProgressBarComponent::compile },
            { TextComponent::TAG_ID, &TextComponent::compile }
        };

        const ViewCallbackMap Views = {
            { GameplayView::VIEW_ID, GameplayView::Callbacks }
        };
    }
}