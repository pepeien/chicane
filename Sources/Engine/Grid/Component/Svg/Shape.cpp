#include "Chicane/Grid/Component/Svg/Shape.hpp"

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    namespace Grid
    {
        SvgShape::SvgShape()
            : Component(TAG_ID)
        {}

        bool SvgShape::isDrawable() const
        {
            if (!hasParent() || !m_parent->isDisplayable() || m_parent->isCulled())
            {
                return false;
            }

            return isVisible() && isSolid() && hasPrimitive();
        }

        void SvgShape::refreshStyleRuleset()
        {
            return;
        }

        void SvgShape::configure(const Primitive& inPrimitive, const Color::Rgba& inColor)
        {
            m_style.background.color.set(inColor);

            setPrimitive(inPrimitive);
        }

        void SvgShape::sync(const Component* inParent, float inScale)
        {
            if (!inParent)
            {
                return;
            }

            setSize(inParent->getSize());
            setPosition(inParent->getPosition());

            Primitive primitive = getPrimitive();
            if (!primitive.outline.empty())
            {
                const float scale    = std::max(std::fabs(inScale), 1e-4f);
                const float dilation = 1.0f / scale;
                primitive.dilation   = dilation;

                const Vec2 min(primitive.outlineMin.x - dilation, primitive.outlineMin.y - dilation);
                const Vec2 max(primitive.outlineMax.x + dilation, primitive.outlineMax.y + dilation);
                const Vec2 center((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);

                setScale(std::max(max.x - min.x, 1e-6f) * scale, std::max(max.y - min.y, 1e-6f) * scale);
                setOffset(center.x * scale, -center.y * scale);

                primitive.indices = {0, 1, 2, 2, 3, 0};
                primitive.vertices.clear();

                auto push = [&](float inX, float inY, float inU, float inV)
                {
                    Vertex vertex;
                    vertex.position.x = inX;
                    vertex.position.y = inY;
                    vertex.uv         = Vec2(inU, inV);
                    primitive.vertices.push_back(vertex);
                };

                push(-0.5f, -0.5f, 0.0f, 0.0f);
                push(0.5f, -0.5f, 1.0f, 0.0f);
                push(0.5f, 0.5f, 1.0f, 1.0f);
                push(-0.5f, 0.5f, 0.0f, 1.0f);

                setPrimitive(primitive);
            }
            else
            {
                setScale(std::max(std::fabs(inScale), 1e-4f), std::max(std::fabs(inScale), 1e-4f));
                setOffset(0.0f, 0.0f);
            }

            refreshBounds();
        }

        void SvgShape::clear()
        {
            clearPrimitive();

            setSize(0.0f, 0.0f);

            m_style.background.color.set(Color::toRgba(Color::TEXT_COLOR_TRANSPARENT));
        }
    }
}
