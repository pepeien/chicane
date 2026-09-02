#include "Chicane/Grid/Component/Dock/Splitter.hpp"

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Grid/Component/Scrollable.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        DockSplitter::DockSplitter()
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

        bool DockSplitter::isDrawable() const
        {
            if (!hasParent() || !m_parent->isDisplayable())
            {
                return false;
            }

            return isVisible() && isSolid() && hasPrimitive();
        }

        float DockSplitter::getDepth() const
        {
            if (!hasParent())
            {
                return 50.0f + m_depthBias;
            }

            return m_parent->getDepth() + 50.0f + m_depthBias;
        }

        Vec2 DockSplitter::getDrawPosition() const
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

        void DockSplitter::configure(
            const Bounds2D& inBox, const Color::Rgba& inColor, WindowCursor inCursor, float inDepthBias
        )
        {
            if (inBox.isEmpty())
            {
                hide();

                return;
            }

            m_depthBias = inDepthBias;
            m_style.background.color.set(inColor);

            if (inCursor == WindowCursor::EwResize)
            {
                m_style.cursor.setRaw(Style::CURSOR_TYPE_EW_RESIZE);
            }
            else if (inCursor == WindowCursor::NsResize)
            {
                m_style.cursor.setRaw(Style::CURSOR_TYPE_NS_RESIZE);
            }
            else
            {
                m_style.cursor.setRaw(Style::CURSOR_TYPE_DEFAULT);
            }

            m_style.cursor.set(inCursor);

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

        void DockSplitter::hide()
        {
            m_depthBias = 0.0f;

            setSize(0.0f, 0.0f);
            setScale(0.0f, 0.0f);
        }
    }
}
