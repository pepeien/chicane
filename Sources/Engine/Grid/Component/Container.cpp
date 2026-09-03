#include "Chicane/Grid/Component/Container.reflected.hpp"

#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    namespace Grid
    {
        Container::Container(const pugi::xml_node& inNode)
            : Scrollable(inNode)
        {
            applyBackgroundPrimitive();
        }

        Container::Container(const String& inTag)
            : Scrollable(inTag)
        {
            applyBackgroundPrimitive();
        }

        void Container::applyBackgroundPrimitive()
        {
            Primitive primitive;
            primitive.indices = {0, 1, 2, 2, 3, 0};

            Vertex vertex;

            // Top Left
            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = -0.5f;
            vertex.position.y = -0.5f;
            primitive.vertices.push_back(vertex);

            // Top right
            vertex.uv.x       = 1.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = 0.5f;
            vertex.position.y = -0.5f;
            primitive.vertices.push_back(vertex);

            // Bottom Right
            vertex.uv.x       = 1.0f;
            vertex.uv.y       = 1.0f;
            vertex.position.x = 0.5f;
            vertex.position.y = 0.5f;
            primitive.vertices.push_back(vertex);

            // Bottom Left
            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 1.0f;
            vertex.position.x = -0.5f;
            vertex.position.y = 0.5f;
            primitive.vertices.push_back(vertex);

            setPrimitive(primitive);
        }
    }
}