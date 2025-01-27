#include "Chicane/Game/Transformable/Component/Camera.hpp"

namespace Chicane
{
    CCamera::CCamera()
        : CView()
    {}

    void CCamera::onTransform()
    {
        CView::onTransform();

        setFocusPoint(getTranslation() + getForward());
    }
}