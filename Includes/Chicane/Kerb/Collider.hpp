#pragma once

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Kerb.hpp"

namespace Chicane
{
    namespace Kerb
    {
        class CHICANE_KERB Collider
        {
        public:
            Collider(const std::vector<Vec3>& inVertices, const std::vector<std::uint32_t>& inIndices);
            Collider();

        private:
            std::vector<Vec3>          m_shapeVertices;
            std::vector<std::uint32_t> m_shapeIndices;
        };
    }
}