#include "Chicane/Runtime/Scene/Component/Camera.reflected.hpp"

namespace Chicane
{
    CCamera::CCamera()
        : CView(),
          lookAt()
    {}

    void CCamera::onLoad()
    {
        applyLookAt(lookAt);
    }

    void CCamera::onPropertyEdited(const String& inName)
    {
        if (inName.equals(LOOK_AT_ATTRIBUTE_NAME))
        {
            applyLookAt(lookAt);
        }
    }

    void CCamera::onResize(const Vec<2, std::uint32_t>& inSize)
    {
        setViewport(inSize);
    }
}