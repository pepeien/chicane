#include "Chicane/Game/Transformable/Component/Light.hpp"

namespace Chicane
{
    CLight::CLight()
        : CView()
    {}

    void CLight::onTransform()
    {
        CView::onTransform();

        setFocusPoint(getTranslation() + getForward());
    }
}