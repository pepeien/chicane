#include "Chicane/Grid/Component/Svg/Shape.hpp"

#include "Chicane/Core/Color.hpp"

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

            const Vec2&        size   = inParent->getSize();
            const StyleRadius& radius = inParent->getStyle().radius;

            setSize(size);
            setScale(inScale, inScale);
            setPosition(inParent->getPosition());

            m_style.radius.x.top.copyValue(radius.x.top);
            m_style.radius.x.right.copyValue(radius.x.right);
            m_style.radius.x.bottom.copyValue(radius.x.bottom);
            m_style.radius.x.left.copyValue(radius.x.left);
            m_style.radius.y.top.copyValue(radius.y.top);
            m_style.radius.y.right.copyValue(radius.y.right);
            m_style.radius.y.bottom.copyValue(radius.y.bottom);
            m_style.radius.y.left.copyValue(radius.y.left);

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
