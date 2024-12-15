#include "Chicane/Box/Assets/Cubemap.hpp"

#include "Chicane/Box/Assets/Texture.hpp"
#include "Chicane/Core/Utils.hpp"
#include "Chicane/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        constexpr auto SIDE_ATTRIBUTE_NAME = "side";

        Cubemap::Cubemap(const std::string& inFilepath)
            : Asset(inFilepath)
        {
            fetchTextures();
        }

        const std::unordered_map<Cubemap::Side, std::string>& Cubemap::getTextures() const
        {
            return m_textures;
        }

        const std::string& Cubemap::getTexture(Cubemap::Side inSide) const
        {
            if (m_textures.find(inSide) == m_textures.end())
            {
                throw std::runtime_error("Cubemap side not found");
            }

            return m_textures.at(inSide);
        }

        Cubemap::Side Cubemap::getSideFromString(const std::string& inValue) const
        {
            std::string value = inValue;
            std::transform(value.begin(), value.end(), value.begin(), ::toupper);

            if (Utils::areEquals(value, "UP"))
            {
                return Side::Up;
            }

            if (Utils::areEquals(value, "DOWN"))
            {
                return Side::Down;
            }

            if (Utils::areEquals(value, "LEFT"))
            {
                return Side::Left;
            }

            if (Utils::areEquals(value, "RIGHT"))
            {
                return Side::Right;
            }

            if (Utils::areEquals(value, "FRONT"))
            {
                return Side::Front;
            }

            return Side::Back;
        }

        void Cubemap::fetchTextures()
        {
            if (m_header.filepath.empty() || m_xml.empty())
            {
                m_textures.clear();

                return;
            }

            for (const auto& texture : m_xml.first_child().children())
            {
                if (!Utils::areEquals(texture.name(), Texture::TAG))
                {
                    continue;
                }

                Side side = getSideFromString(
                    XML::getAttribute(
                        SIDE_ATTRIBUTE_NAME,
                        texture
                    ).as_string()
                );

                if (m_textures.find(side) != m_textures.end())
                {
                    throw std::runtime_error("There are duplicated sides inside the " + m_header.id + " cubemap");
                }

                m_textures.insert(
                    std::make_pair(
                        side,
                        Utils::trim(texture.child_value())
                    )
                );
            }

            if (m_textures.size() < 6)
            {
                throw std::runtime_error("There cubemap " + m_header.id + " have insuficient sides");
            }
        }
    }
}