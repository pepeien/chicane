#include "Chicane/Grid/Component/Dock/Drop.hpp"

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Grid/Component/Scrollable.hpp"

namespace Chicane
{
    namespace Grid
    {
        DockDrop::DockDrop()
            : Component(TAG_ID),
              m_depthBias(0.0f)
        {
            Primitive primitive = {};
            primitive.indices   = {0, 1, 2, 2, 3, 0};

            Vertex vertex     = {};
            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = -0.5f;
            vertex.position.y = -0.5f;
            primitive.vertices.push_back(vertex);

            vertex.uv.x       = 1.0f;
            vertex.position.x = 0.5f;
            primitive.vertices.push_back(vertex);

            vertex.uv.y       = 1.0f;
            vertex.position.y = 0.5f;
            primitive.vertices.push_back(vertex);

            vertex.uv.x       = 0.0f;
            vertex.position.x = -0.5f;
            primitive.vertices.push_back(vertex);

            setPrimitive(primitive);
        }

        bool DockDrop::isDrawable() const
        {
            if (!hasParent() || !m_parent->isDisplayable())
            {
                return false;
            }

            return isVisible() && isSolid() && hasPrimitive();
        }

        float DockDrop::getDepth() const
        {
            if (!hasParent())
            {
                return 80.0f + m_depthBias;
            }

            return m_parent->getDepth() + 200.0f + m_depthBias;
        }

        Vec2 DockDrop::getDrawPosition() const
        {
            Vec2 result = getPosition();

            const Component* ancestor = hasParent() ? m_parent->getParent() : nullptr;
            while (ancestor && ancestor != this)
            {
                if (const Scrollable* scrollable = dynamic_cast<const Scrollable*>(ancestor))
                {
                    result.x -= scrollable->getScroll().x;
                    result.y -= scrollable->getScroll().y;
                }

                if (ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return result;
        }

        void DockDrop::configure(const Bounds2D& inBox, const Color::Rgba& inColor)
        {
            if (inBox.isEmpty())
            {
                hide();

                return;
            }

            m_depthBias = 0.0f;
            m_style.background.color.set(inColor);

            const float width  = inBox.right - inBox.left;
            const float height = inBox.bottom - inBox.top;

            setSize(width, height);
            setScale(width, height);

            if (!hasParent())
            {
                return;
            }

            const Vec2& origin = m_parent->getPosition();
            setPosition(origin.x + inBox.left, origin.y + inBox.top);
        }

        void DockDrop::hide()
        {
            m_depthBias = 0.0f;

            setSize(0.0f, 0.0f);
            setScale(0.0f, 0.0f);
        }
    }
}
