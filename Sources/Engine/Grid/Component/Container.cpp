#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        Container::Container(const pugi::xml_node& inNode)
            : Component(inNode)
        {
            Primitive primitive = {};
            primitive.indices   = {0, 1, 2, 2, 3, 0};

            Vertex vertex = {};

            // Bottom-left
            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = -1.0f;
            vertex.position.y = -1.0f;
            primitive.vertices.push_back(vertex);

            // Bottom-right
            vertex.uv.x       = 1.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = 1.0f;
            vertex.position.y = -1.0f;
            primitive.vertices.push_back(vertex);

            // Top-right
            vertex.uv.x       = 1.0f;
            vertex.uv.y       = 1.0f;
            vertex.position.x = 1.0f;
            vertex.position.y = 1.0f;
            primitive.vertices.push_back(vertex);

            // Top-left
            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 1.0f;
            vertex.position.x = -1.0f;
            vertex.position.y = 1.0f;
            primitive.vertices.push_back(vertex);

            setPrimitive(primitive);
        }
    }
}