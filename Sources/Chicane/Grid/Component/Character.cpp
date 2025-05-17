#include "Chicane/Grid/Component/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        static const Box::Font::ParsedData EMPTY_FONT  = {};
        static const Box::Font::Glyph      EMPTY_GLYPH = {};

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

                    const Style::Instance& style = m_parent->getStyle();

                    m_style.foregroundColor = style.foregroundColor;
                    m_style.font            = style.font;
                    m_style.width           = m_style.font.size;
                    m_style.height          = m_style.font.size;

                    if (!hasGlyph())
                    {
                        return;
                    }

                    const Box::Font::Glyph& glyph = getGlyph();
                    //m_style.margin.top    = Style::AUTO_SIZE_UNIT;
                    //m_style.margin.right  = String::sprint("-%d%s", static_cast<int>(glyph.bearing.x), Style::PIXEL_SIZE_UNIT);
                    //m_style.margin.bottom = String::sprint("-%d%s", static_cast<int>(glyph.bearing.y), Style::PIXEL_SIZE_UNIT);
                    //m_style.margin.left   = Style::AUTO_SIZE_UNIT;

                    m_style.emmitChanges();
                }
            );
        }

        bool Character::isDrawable() const
        {
            return isVisible() && hasGlyph() && !isPrimitiveEmpty();
        }

        void Character::refreshPrimitive()
        {
            m_primitive.clear();

            if (!hasGlyph())
            {
                return;
            }

            const std::vector<Vec<3, float>>& vertices = getGlyph().vertices;
            const std::vector<std::uint32_t>& indices  = getGlyph().indices;

            Vertex vertex = {};
            vertex.color  = m_style.foregroundColor;

            for (std::uint32_t index : indices)
            {
                vertex.position = vertices.at(index);
    
                m_primitive.push_back(vertex);
            }
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
        }

        bool Character::hasFont() const
        {
            return Box::getFontManager()->isFamilyLoaded(m_style.font.family);
        }

        const Box::Font::ParsedData& Character::getFont() const
        {
            if (!hasFont())
            {
                return EMPTY_FONT;
            }

            return Box::getFontManager()->getByFamily(m_style.font.family);
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
    }
}