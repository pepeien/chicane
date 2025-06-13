#include "Chicane/Box/Asset/Instance.hpp"

#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            Instance::Instance(const FileSystem::Path& inSource)
            {
                if (inSource.empty())
                {
                    return;
                }

                if (FileSystem::exists(inSource))
                {
                    fetchXML(inSource);
                }
                else
                {
                    createXML(inSource);
                    saveXML();
                }

                m_header = Header(inSource);
            }

            bool Instance::isType(Type inType) const
            {
                return m_header.type == inType;
            }

            bool Instance::isEmpty() const
            {
                pugi::xml_node root = getXML();

                if (Xml::isEmpty(root))
                {
                    return true;
                }

                return root.first_child() == root.last_child() && Xml::isEmpty(root.first_child());
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

            const FileSystem::Path& Instance::getFilepath() const
            {
                return m_header.filepath;
            }

            void Instance::setFilepath(const FileSystem::Path& inFilepath)
            {
                m_header.filepath = inFilepath;
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

            const String& Instance::getId() const
            {
                return m_header.id;
            }

            void Instance::setId(const String& inId)
            {
                if (inId.isEmpty())
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

            void Instance::saveXML()
            {
                Xml::save(m_xml, getFilepath().string());
            }

            pugi::xml_node Instance::getXML() const
            {
                return m_xml.first_child();
            }

            void Instance::createXML(const FileSystem::Path& inFilepath)
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

            void Instance::fetchXML(const FileSystem::Path& inFilepath)
            {
                m_xml = Xml::load(inFilepath);

                pugi::xml_node root = getXML();

                bool bIsRoot  = root.parent() == root.root();
                bool bIsAlone = bIsRoot && !root.next_sibling();

                if (!bIsRoot || !bIsAlone)
                {
                    throw std::runtime_error("Asset files root element must not have any siblings");
                }

                const String name = root.name();

                if (!name.equals(TAG))
                {
                    throw std::runtime_error(
                        String::sprint(
                            "Asset files root element must be have [%s] as a tag", TAG
                        )
                    );
                }
            }
        }
    }
}