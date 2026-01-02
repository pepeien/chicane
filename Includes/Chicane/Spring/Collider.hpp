#pragma once

#include "Chicane/Spring/Essential.hpp"

namespace Chicane
{
    namespace Spring
    {
        class CHICANE_SPRING Collider
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