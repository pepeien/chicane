#include "Chicane/Game/Transformable/Component/LightComponent.hpp"

namespace Chicane
{
    LightComponent::LightComponent()
        : ViewComponent()
    {}

    void LightComponent::onTransform()
    {
        ViewComponent::onTransform();

        setFocusPoint(getTranslation() + getForward());
    }
}