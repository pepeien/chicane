#include "Chicane/Runtime/Scene/Component/Camera.reflected.hpp"

namespace Chicane
{
    CCamera::CCamera()
        : CView()
    {}

    void CCamera::onResize(const Vec<2, std::uint32_t>& inSize)
    {
        setViewport(inSize);
    }

    Transform2D CCamera::projectToScreenSpace(
        const Object* inTarget, const Vec2& inViewport, const Vec3& inOffset
    ) const
    {
        if (inTarget == nullptr)
        {
            return {};
        }

        return inTarget->toScreenSpace(m_data.view, m_data.projection, inViewport, inOffset);
    }
}
