#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        Container::Container(const pugi::xml_node& inNode)
            : Component(inNode),
            m_canUpdate(false)
        {}

        void Container::onRefresh()
        {
            refreshColor();
        }

        void Container::refreshPrimitive()
        {
            if (!m_canUpdate)
            {
                return;
            }

            m_primitive.clear();

            if (!Color::isVisible(m_color))
            {
                return;
            }

            Vertex vertex = {};
            vertex.color.r = m_color.r / 255.0f;
            vertex.color.g = m_color.g / 255.0f;
            vertex.color.b = m_color.b / 255.0f;
            vertex.color.a = m_color.a / 255.0f;

            vertex.position.x = -1.0f;
            vertex.position.y = -1.0f;
            m_primitive.vertices.push_back(vertex);

            vertex.position.x =  1.0f;
            vertex.position.y = -1.0f;
            m_primitive.vertices.push_back(vertex);

            vertex.position.x = 1.0f;
            vertex.position.y = 1.0f;
            m_primitive.vertices.push_back(vertex);

            vertex.position.x = -1.0f;
            vertex.position.y =  1.0f;
            m_primitive.vertices.push_back(vertex);

            m_primitive.indices = { 0, 1, 2, 2, 3, 0 };

            m_canUpdate = false;

            emmitChanges();
        }

        void Container::refreshColor()
        {
            if (Color::areEquals(m_color, m_style.backgroundColor))
            {
                return;
            }

            m_color = m_style.backgroundColor;

            m_canUpdate = true;

            refreshPrimitive();
        }
    }
}