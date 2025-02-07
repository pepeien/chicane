#include "Chicane/Game/Transformable/Component/Camera.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Game/Transformable/Component/Mesh.hpp"

namespace Chicane
{
    CCamera::CCamera()
        : CView()
    {}

    void CCamera::onTransform()
    {
        CView::onTransform();

        setFocusPoint(getTranslation() + getForward());

        for (CMesh* mesh : Application::getLevel()->getComponents<CMesh>())
        {
            if (canSee(mesh))
            {
                mesh->show();
            }
            else
            {
                mesh->hide();
            }
        }
    }
}