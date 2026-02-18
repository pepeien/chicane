#include "Chicane/Grid/Component/Container.hpp"

#include "Chicane/Core/Log.hpp"

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

            // Top Left
            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = -1.0f;
            vertex.position.y = -1.0f;
            primitive.vertices.push_back(vertex);

            // Top right
            vertex.uv.x       = 1.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = 1.0f;
            vertex.position.y = -1.0f;
            primitive.vertices.push_back(vertex);

            // Bottom Right
            vertex.uv.x       = 1.0f;
            vertex.uv.y       = 1.0f;
            vertex.position.x = 1.0f;
            vertex.position.y = 1.0f;
            primitive.vertices.push_back(vertex);

            // Bottom Left
            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 1.0f;
            vertex.position.x = -1.0f;
            vertex.position.y = 1.0f;
            primitive.vertices.push_back(vertex);

            setPrimitive(primitive);
        }

        void Container::onRefresh()
        {
            Component::onRefresh();

            if (m_style.height.isRaw(Size::AUTO_KEYWORD) || m_style.width.isRaw(Size::AUTO_KEYWORD))
            {
                return;
            }

            if (!m_style.align.isRaw(Style::ALIGNMENT_TYPE_CENTER))
            {
                return;
            }

            Vec2 min(FLT_MAX);
            Vec2 max(-FLT_MAX);

            for (auto* child : m_children)
            {
                const Vec2& pos  = child->getPosition();
                const Vec2& size = child->getSize();

                min.x = std::min(min.x, pos.x);
                min.y = std::min(min.y, pos.y);

                max.x = std::max(max.x, pos.x + size.x);
                max.y = std::max(max.y, pos.y + size.y);
            }

            const Vec2 contentSize = max - min;

            m_style.padding.left = (m_size.x - contentSize.x) * 0.5f;
            m_style.padding.top  = (m_size.y - contentSize.y) * 0.5f;
        }
    }
}