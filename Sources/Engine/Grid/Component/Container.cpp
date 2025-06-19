#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        Container::Container(const pugi::xml_node& inNode)
            : Super(inNode),
            m_bCanUpdate(false)
        {}

        void Container::onRefresh()
        {
            refreshColor();
        }

        void Container::refreshPrimitive()
        {
            if (!m_bCanUpdate)
            {
                return;
            }

            if (!Color::isVisible(m_color))
            {
                clearPrimitive();

                return;
            }

            m_bCanUpdate = false;

            Primitive primitive = {};
            primitive.indices = { 0, 1, 2, 2, 3, 0 };

            Vertex vertex = {};
            vertex.color.r = m_color.r / 255.0f;
            vertex.color.g = m_color.g / 255.0f;
            vertex.color.b = m_color.b / 255.0f;
            vertex.color.a = m_color.a / 255.0f;

            vertex.position.x = -1.0f;
            vertex.position.y = -1.0f;
            primitive.vertices.push_back(vertex);

            vertex.position.x =  1.0f;
            vertex.position.y = -1.0f;
            primitive.vertices.push_back(vertex);

            vertex.position.x = 1.0f;
            vertex.position.y = 1.0f;
            primitive.vertices.push_back(vertex);

            vertex.position.x = -1.0f;
            vertex.position.y =  1.0f;
            primitive.vertices.push_back(vertex);

            setPrimitive(primitive);
        }

        void Container::refreshColor()
        {
            if (Color::areEquals(m_color, m_style.backgroundColor))
            {
                return;
            }

            m_color = m_style.backgroundColor;

            m_bCanUpdate = true;
        }
    }
}