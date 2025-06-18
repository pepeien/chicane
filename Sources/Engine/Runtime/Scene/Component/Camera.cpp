#include "Chicane/Runtime/Scene/Component/Camera.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

namespace Chicane
{
    CCamera::CCamera()
        : CView()
    {}

    void CCamera::onTransform()
    {
        CView::onTransform();

        setFocusPoint(getTranslation() + getForward());

        for (CMesh* mesh : Application::getScene()->getComponents<CMesh>())
        {
            if (canSee(mesh))
            {
                mesh->show();
            }
            else
            {
                //mesh->hide();
            }
        }
    }
}