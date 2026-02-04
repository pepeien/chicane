#include "Chicane/Runtime/Scene/Component/Camera.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    CCamera::CCamera()
        : CView()
    {}

    void CCamera::onResize(const Vec<2, std::uint32_t>& inSize)
    {
        setViewport(inSize);
    }
}