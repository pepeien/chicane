#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        Container::Container(const pugi::xml_node& inNode)
            : Component(inNode)
        {}

        std::vector<Vertex> Container::getPrimitive() const
        {
            std::vector<Vertex> result = {};

            const Vec<4, std::uint32_t> color = Color::toRgba(
                getBackgroundColorStyle()
            );

            Vertex vertex = {};
            vertex.color.r = color.r / 255.0f;
            vertex.color.g = color.g / 255.0f;
            vertex.color.b = color.b / 255.0f;
            vertex.color.a = color.a / 255.0f;

            vertex.position.x = -1.0f;
            vertex.position.y = -1.0f;
            result.push_back(vertex);

            vertex.position.x =  1.0f;
            vertex.position.y = -1.0f;
            result.push_back(vertex);

            vertex.position.x = 1.0f;
            vertex.position.y = 1.0f;
            result.push_back(vertex);

            vertex.position.x = 1.0f;
            vertex.position.y = 1.0f;
            result.push_back(vertex);

            vertex.position.x = -1.0f;
            vertex.position.y =  1.0f;
            result.push_back(vertex);

            vertex.position.x = -1.0f;
            vertex.position.y = -1.0f;
            result.push_back(vertex);

            return result;
        }
    }
}