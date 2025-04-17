#include "Box/Asset/Sky/Instance.hpp"

#include "Box/Asset/Model.hpp"
#include "Box/Asset/Texture.hpp"
#include "Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Sky
        {
            static constexpr const char* SIDES_TAG_NAME              = "Sides";
            static constexpr const char* TEXTURE_SIDE_ATTRIBUTE_NAME = "side";

            static const std::map<std::string, Side> SIDE_MAP {
                { "UP",    Side::Up },
                { "DOWN",  Side::Down },
                { "LEFT",  Side::Left },
                { "RIGHT", Side::Right },
                { "FRONT", Side::Front },
                { "BACK",  Side::Back }
            };

            Instance::Instance(const std::string& inFilepath)
                : Asset::Instance(inFilepath)
            {
                fetchSides();
                fetchModel();
            }

            const RawSides& Instance::getSides() const
            {
                return m_sides;
            }

            const std::string& Instance::getSide(Side inSide) const
            {
                if (m_sides.find(inSide) == m_sides.end())
                {
                    throw std::runtime_error("Cubemap side not found");
                }

                return m_sides.at(inSide);
            }

            void Instance::setSide(Side inSide, const RawSide& inFilepath)
            {
                if (inFilepath.empty())
                {
                    return;
                }

                std::string filepath = String::trim(inFilepath);

                if (!FileSystem::exists(filepath))
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

                std::string sideID = side->first;

                pugi::xml_node sideNode = getXMLRoot().find_child_by_attribute(
                    TEXTURE_SIDE_ATTRIBUTE_NAME,
                    sideID.c_str()
                );

                if (XML::isEmpty(sideNode))
                {
                    sideNode = getXMLRoot().append_child(Texture::TAG);
                    sideNode.append_attribute(TEXTURE_SIDE_ATTRIBUTE_NAME).set_value(sideID.c_str());
                }

                sideNode.text().set(filepath);
            }

            const std::string& Instance::getModel() const
            {
                return m_model;
            }

            void Instance::setModel(const std::string& inModel)
            {
                m_model = inModel;
            }

            Side Instance::getSideFromString(const std::string& inValue) const
            {
                std::string value = inValue;
                std::transform(value.begin(), value.end(), value.begin(), ::toupper);

                if (SIDE_MAP.find(value) == SIDE_MAP.end())
                {
                    return Side::Back;
                }

                return SIDE_MAP.at(value);
            }

            void Instance::fetchSides()
            {
                if (getFilepath().empty() || isXMLEmpty())
                {
                    m_sides.clear();

                    return;
                }

                const auto& root = getXMLRoot();

                for (const auto& texture : root.child(SIDES_TAG_NAME).children(Texture::TAG))
                {
                    Side side = getSideFromString(
                        XML::getAttribute(
                            TEXTURE_SIDE_ATTRIBUTE_NAME,
                            texture
                        ).as_string()
                    );

                    if (m_sides.find(side) != m_sides.end())
                    {
                        throw std::runtime_error("There are duplicated sides inside the " + m_header.id + " cube map");
                    }

                    m_sides.insert(
                        std::make_pair(
                            side,
                            String::trim(texture.child_value())
                        )
                    );
                }

                if (m_sides.size() < SIDE_MAP.size())
                {
                    throw std::runtime_error("The sky " + m_header.id + " have insuficient sides");
                }
            }

            void Instance::fetchModel()
            {
                const auto& model = getXMLRoot().child(Model::TAG);

                m_model = String::trim(model.child_value());
            }
        }
    }
}