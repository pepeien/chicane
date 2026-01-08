#include "Chicane/Kerb/Collider.hpp"

#include <Jolt/Jolt.h>

namespace Chicane
{
    namespace Kerb
    {
        Collider::Collider(
            const std::vector<Vec3>& inVertices,
            const std::vector<std::uint32_t>& inIndices
        )
            : m_shapeVertices(inVertices),
            m_shapeIndices(inIndices)
        {}

        Collider::Collider()
            : m_shapeVertices({}),
            m_shapeIndices({})
        {}
    }
}