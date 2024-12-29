#include "Chicane/Box/Assets/CubeMap.hpp"

#include "Chicane/Box/Assets/Texture.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static constexpr auto SIDE_ATTRIBUTE_NAME = "side";

        const std::unordered_map<std::string, CubeMap::Side> SIDE_MAP {
            { "UP",    CubeMap::Side::Up },
            { "DOWN",  CubeMap::Side::Down },
            { "LEFT",  CubeMap::Side::Left },
            { "RIGHT", CubeMap::Side::Right },
            { "FRONT", CubeMap::Side::Front },
            { "BACK",  CubeMap::Side::Back }
        };

        CubeMap::CubeMap(const std::string& inFilepath)
            : Asset(inFilepath)
        {
            fetchTextures();
        }

        const std::unordered_map<CubeMap::Side, std::string>& CubeMap::getTextures() const
        {
            return m_textures;
        }

        const std::string& CubeMap::getTexture(CubeMap::Side inSide) const
        {
            if (m_textures.find(inSide) == m_textures.end())
            {
                throw std::runtime_error("Cubemap side not found");
            }

            return m_textures.at(inSide);
        }

        void CubeMap::setTexture(CubeMap::Side inSide, const std::string& inFilepath)
        {
            if (inFilepath.empty())
            {
                return;
            }

            std::string filepath = Utils::trim(inFilepath);

            if (!FileSystem::exists(filepath))
            {
                throw std::runtime_error("The texture " + filepath + " doesn't exist");
            }

            m_textures[inSide] = filepath;

            auto side = std::find_if(
                SIDE_MAP.begin(),
                SIDE_MAP.end(),
                [inSide](const auto& inPair) { return inPair.second == inSide; }
            );

            if (side == SIDE_MAP.end())
            {
                return;
            }

            std::string sideID = side->first;

            pugi::xml_node sideNode = getXMLRoot().find_child_by_attribute(
                SIDE_ATTRIBUTE_NAME,
                sideID.c_str()
            );

            if (XML::isEmpty(sideNode))
            {
                sideNode = getXMLRoot().append_child(Texture::TAG);
                sideNode.append_attribute(SIDE_ATTRIBUTE_NAME).set_value(sideID.c_str());
            }

            sideNode.text().set(filepath);
        }

        CubeMap::Side CubeMap::getSideFromString(const std::string& inValue) const
        {
            std::string value = inValue;
            std::transform(value.begin(), value.end(), value.begin(), ::toupper);

            if (SIDE_MAP.find(value) == SIDE_MAP.end())
            {
                return Side::Back;
            }

            return SIDE_MAP.at(value);
        }

        void CubeMap::fetchTextures()
        {
            if (getFilepath().empty() || isXMLEmpty())
            {
                m_textures.clear();

                return;
            }

            for (const auto& texture : getXMLRoot().children())
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