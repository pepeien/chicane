#include "Chicane/Box/Asset/Sky/Instance.hpp"

#include "Chicane/Box/Asset/Model.hpp"
#include "Chicane/Box/Asset/Texture.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sky
        {
            static inline constexpr const char* SIDES_TAG_NAME              = "Sides";
            static inline constexpr const char* TEXTURE_SIDE_ATTRIBUTE_NAME = "side";

            static const std::map<String, Side> SIDE_MAP {
                { "UP",    Side::Up },
                { "DOWN",  Side::Down },
                { "LEFT",  Side::Left },
                { "RIGHT", Side::Right },
                { "FRONT", Side::Front },
                { "BACK",  Side::Back }
            };

            Instance::Instance(const FileSystem::Path& inFilepath)
                : Asset::Instance(inFilepath)
            {
                fetchSides();
                fetchModel();
            }

            const RawSides& Instance::getSides() const
            {
                return m_sides;
            }

            const String& Instance::getSide(Side inSide) const
            {
                if (m_sides.find(inSide) == m_sides.end())
                {
                    throw std::runtime_error("Cubemap side not found");
                }

                return m_sides.at(inSide);
            }

            void Instance::setSide(Side inSide, const RawSide& inFilepath)
            {
                if (inFilepath.isEmpty())
                {
                    return;
                }

                const String filepath = inFilepath.trim();

                if (!FileSystem::exists(filepath.toStandard()))
                {
                    throw std::runtime_error("The texture " + filepath + " doesn't exist");
                }

                m_sides[inSide] = filepath;

                auto side = std::find_if(
                    SIDE_MAP.begin(),
                    SIDE_MAP.end(),
                    [inSide](const auto& inPair) { return inPair.second == inSide; }
                );

                if (side == SIDE_MAP.end())
                {
                    return;
                }

                String sideID = side->first;

                pugi::xml_node sideNode = getXML().find_child_by_attribute(
                    TEXTURE_SIDE_ATTRIBUTE_NAME,
                    sideID.toChar()
                );

                if (Xml::isEmpty(sideNode))
                {
                    sideNode = getXML().append_child(Texture::TAG);
                    sideNode.append_attribute(TEXTURE_SIDE_ATTRIBUTE_NAME).set_value(sideID.toChar());
                }

                sideNode.text().set(filepath);
            }

            const String& Instance::getModel() const
            {
                return m_model;
            }

            void Instance::setModel(const String& inModel)
            {
                m_model = inModel;
            }

            Side Instance::getSideFromString(const String& inValue) const
            {
                String value = inValue;
                std::transform(value.begin(), value.end(), value.begin(), ::toupper);

                if (SIDE_MAP.find(value) == SIDE_MAP.end())
                {
                    return Side::Back;
                }

                return SIDE_MAP.at(value);
            }

            void Instance::fetchSides()
            {
                if (getFilepath().empty() || isEmpty())
                {
                    m_sides.clear();

                    return;
                }

                const auto& root = getXML();

                for (const auto& texture : root.child(SIDES_TAG_NAME).children(Texture::TAG))
                {
                    Side side = getSideFromString(
                        Xml::getAttribute(
                            TEXTURE_SIDE_ATTRIBUTE_NAME,
                            texture
                        ).as_string()
                    );

                    if (m_sides.find(side) != m_sides.end())
                    {
                        throw std::runtime_error("There are duplicated sides inside the " + m_header.id + " cube map");
                    }

                    const String textureName = texture.child_value();

                    m_sides.insert(std::make_pair(side, textureName.trim()));
                }

                if (m_sides.size() < SIDE_MAP.size())
                {
                    throw std::runtime_error("The sky " + m_header.id + " have insuficient sides");
                }
            }

            void Instance::fetchModel()
            {
                const auto& model = getXML().child(Model::TAG);

                m_model = model.child_value();
                m_model = m_model.trim();
            }
        }
    }
}