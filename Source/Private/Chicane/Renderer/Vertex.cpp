#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Vertex
    {
        Vec<3, float> generateExtent(const std::vector<Vertex::Instance>& inVertices)
        {
            float xMin = FLT_MAX;
            float xMax = -FLT_MAX;

            float yMin = FLT_MAX;
            float yMax = -FLT_MAX;

            float zMin = FLT_MAX;
            float zMax = -FLT_MAX;

            for (const Vertex::Instance& vertex : inVertices)
            {
                const Vec<3, float>& position = vertex.position;

                if (position.x < xMin)
                {
                    xMin = position.x;
                }

                if (position.x > xMax)
                {
                    xMax = position.x;
                }

                if (position.y < yMin)
                {
                    yMin = position.y;
                }

                if (position.y > yMax)
                {
                    yMax = position.y;
                }

                if (position.z < zMin)
                {
                    zMin = position.z;
                }

                if (position.z > zMax)
                {
                    zMax = position.z;
                }
            }

            Vec<3, float> result {};
            result.x = std::abs(xMax) + std::abs(xMin);
            result.y = std::abs(yMax) + std::abs(yMin);
            result.z = std::abs(zMax) + std::abs(zMin);

            return result;
        }
    }
}