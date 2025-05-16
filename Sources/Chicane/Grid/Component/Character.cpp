#include "Chicane/Grid/Component/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        static const Box::Font::RawData EMPTY_FONT  = {};
        static const Box::Font::Glyph   EMPTY_GLYPH = {};

        Character::Character()
            : Component(TAG_ID),
            m_character('\0')
        {
            Box::getFontManager()->watchChanges(
                [this](Box::Manager::EventType event)
                {
                    if (event != Box::Manager::EventType::Load)
                    {
                        return;
                    }

                    refreshStyle();
                }
            );
        }

        bool Character::isDrawable() const
        {
            return isVisible() && hasGlyph();
        }

        std::vector<Vertex> Character::getPrimitive() const
        {
            if (!hasGlyph())
            {
                return {};
            }

            std::vector<Vertex> result = {};

            Vertex vertex = {};
            vertex.color  = Color::toRgba(
                getForegroundColorStyle()
            );

            for (const Vec<3, float>& points : getGlyph().vertices)
            {
                vertex.position = points;

                result.push_back(vertex);
            }

            return result;
        }

        char Character::getCharacter() const
        {
            return m_character;
        }

        void Character::setCharacter(char inValue)
        {
            if (m_character == inValue || inValue == '\0')
            {
                return;
            }

            m_character = inValue;

            refreshStyle();
        }

        bool Character::hasFont() const
        {
            return Box::getFontManager()->isFamilyLoaded(getFontFamilyStyle());
        }

        const Box::Font::RawData& Character::getFont() const
        {
            if (!hasFont())
            {
                return EMPTY_FONT;
            }

            return Box::getFontManager()->getByFamily(getFontFamilyStyle());
        }

        bool Character::hasGlyph() const
        {
            return hasFont() && getFont().hasGlyph(m_character);
        }

        const Box::Font::Glyph& Character::getGlyph() const
        {
            if (!hasGlyph())
            {
                return EMPTY_GLYPH;
            }

            return getFont().getGlyph(m_character);
        }

        void Character::refreshStyle()
        {
            const Style::Instance& style = m_parent->getStyle();

            m_style.foregroundColor = style.foregroundColor;
            m_style.font            = style.font;

            if (!hasGlyph())
            {
                return;
            }

            const Box::Font::Glyph& glyph = getGlyph();

            m_style.width         = m_style.font.size;
            m_style.height        = m_style.font.size;
            //m_style.margin.top    = Style::AUTO_SIZE_UNIT;
            //m_style.margin.right  = String::sprint("-%d%s", static_cast<int>(glyph.bearing.x), Style::PIXEL_SIZE_UNIT);
            //m_style.margin.bottom = String::sprint("-%d%s", static_cast<int>(glyph.bearing.y), Style::PIXEL_SIZE_UNIT);
            //m_style.margin.left   = Style::AUTO_SIZE_UNIT;
        }
    }
}