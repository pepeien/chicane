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
            if (!hasParent() || !m_parent->isDisplayable())
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

            const Vec2& size = inParent->getSize();

            setSize(size);
            setScale(inScale, inScale);
            setPosition(inParent->getPosition());

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
