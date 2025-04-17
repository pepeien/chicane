#include "Box/Asset/Instance.hpp"

#include "Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            std::string Instance::TAG = "Asset";

            Instance::Instance(const std::string& inFilepath)
            {
                std::string filepath = String::trim(inFilepath);

                if (filepath.empty())
                {
                    return;
                }

                if (FileSystem::exists(filepath))
                {
                    fetchXML(filepath);
                    fetchHeader(filepath);

                    return;
                }

                createXML(filepath);
                fetchHeader(filepath);
                saveXML();
            }

            bool Instance::isType(Type inType) const
            {
                return m_header.type == inType;
            }

            const Header& Instance::getHeader() const
            {
                return m_header;
            }

            void Instance::setHeader(const Header& inHeader)
            {
                setFilepath(inHeader.filepath);
                setVersion(inHeader.version);
                setId(inHeader.id);
                setType(inHeader.type);
            }

            const std::filesystem::path& Instance::getFilepath() const
            {
                return m_header.filepath;
            }

            void Instance::setFilepath(const std::filesystem::path& inFilepath)
            {
                m_header.filepath = String::trim(inFilepath.string());
            }

            std::uint32_t Instance::getVersion() const
            {
                return m_header.version;
            }

            void Instance::setVersion(std::uint32_t inVersion)
            {
                if (inVersion <= 0)
                {
                    return;
                }

                m_header.version = inVersion;

                m_xml
                .first_child()
                .attribute(VERSION_ATTRIBUTE_NAME)
                .set_value(inVersion);
            }

            const std::string& Instance::getId() const
            {
                return m_header.id;
            }

            void Instance::setId(const std::string& inId)
            {
                if (inId.empty())
                {
                    return;
                }

                m_header.id = inId;

                m_xml
                .first_child()
                .attribute(ID_ATTRIBUTE_NAME)
                .set_value(inId);
            }

            Type Instance::getType() const
            {
                return m_header.type;
            }

            void Instance::setType(Type inType)
            {
                m_header.type = inType;
            }

            bool Instance::isXMLEmpty()
            {
                pugi::xml_node root = getXMLRoot();

                if (XML::isEmpty(root))
                {
                    return true;
                }

                return root.first_child() == root.last_child() && XML::isEmpty(root.first_child());
            }

            pugi::xml_node Instance::getXMLRoot()
            {
                return m_xml.first_child();
            }

            void Instance::createXML(const std::string& inFilepath)
            {
                if (inFilepath.empty() || !m_xml.children().empty())
                {
                    return;
                }

                pugi::xml_node root = m_xml.append_child(TAG);

                root
                .append_attribute(VERSION_ATTRIBUTE_NAME)
                .set_value(CURRENT_VERSION);
            }

            void Instance::fetchXML(const std::string& inFilepath)
            {
                XML::load(m_xml, inFilepath);

                pugi::xml_node root = getXMLRoot();

                bool bIsRoot  = root.parent() == root.root();
                bool bIsAlone = bIsRoot && !root.next_sibling();

                if (!bIsRoot || !bIsAlone)
                {
                    throw std::runtime_error("Asset files root element must not have any siblings");
                }

                if (!String::areEquals(TAG, root.name()))
                {
                    throw std::runtime_error("Asset files root element must be a " + TAG);
                }
            }

            void Instance::saveXML()
            {
                XML::save(m_xml, getFilepath().string());
            }

            void Instance::fetchHeader(const std::string& inFilepath)
            {
                if (inFilepath.empty())
                {
                    return;
                }

                m_header = Header::fromXML(inFilepath, m_xml);
            }
        }
    }
}