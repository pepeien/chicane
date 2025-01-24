#include "Chicane/Game/Transformable/Component/CameraComponent.hpp"

namespace Chicane
{
    CameraComponent::CameraComponent()
        : ViewComponent()
    {}

    void CameraComponent::onTransform()
    {
        ViewComponent::onTransform();

        setFocusPoint(getTranslation() + getForward());
    }
}