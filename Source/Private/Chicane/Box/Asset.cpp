#include "Chicane/Box/Asset.hpp"

#include "Chicane/Box/Assets/Cubemap.hpp"
#include "Chicane/Box/Assets/Mesh.hpp"
#include "Chicane/Box/Assets/Model.hpp"
#include "Chicane/Box/Assets/Texture.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Xml.hpp"

const std::unordered_map<std::string, Chicane::Box::Asset::Type> TYPES {
    { Chicane::Box::Cubemap::EXTENSION, Chicane::Box::Asset::Type::Cubemap },
    { Chicane::Box::Mesh::EXTENSION, Chicane::Box::Asset::Type::Mesh },
    { Chicane::Box::Model::EXTENSION, Chicane::Box::Asset::Type::Model },
    { Chicane::Box::Texture::EXTENSION, Chicane::Box::Asset::Type::Texture }
};

namespace Chicane
{
    namespace Box
    {
        std::string   Asset::TAG             = "Asset";
        std::uint32_t Asset::CURRENT_VERSION = 1;

        constexpr auto VERSION_ATTRIBUTE_NAME = "version";
        constexpr auto ID_ATTRIBUTE_NAME      = "id";

        Asset::Header Asset::Header::fromFilepath(const std::string& inFilepath)
        {
            if (inFilepath.empty())
            {
                return {};
            }

            pugi::xml_document xml;

            XML::load(xml, inFilepath);

            return Header::fromXML(inFilepath, xml);
        }

        Asset::Header Asset::Header::fromXML(const std::string& inFilepath, const pugi::xml_document& inDocument)
        {
            const pugi::xml_node root = inDocument.first_child();

            Header header {};
            header.filepath = inFilepath;
            header.fetchVersion(root);
            header.fetchId(root);
            header.fetchType();

            return header;
        }

        void Asset::Header::fetchVersion(const pugi::xml_node& inRoot)
        {
            if (inRoot.empty())
            {
                version = CURRENT_VERSION;

                return;
            }

            version = XML::getAttribute(VERSION_ATTRIBUTE_NAME, inRoot).as_uint();

            if (!version > 0)
            {
                return;
            }

            version = CURRENT_VERSION;
        }

        void Asset::Header::fetchId(const pugi::xml_node& inRoot)
        {
            if (inRoot.empty())
            {
                id = "";

                return;
            }

            id = XML::getAttribute(ID_ATTRIBUTE_NAME, inRoot).as_string();

            if (!id.empty())
            {
                return;
            }

            id = "";
        }

        void Asset::Header::fetchType()
        {
            if (filepath.empty())
            {
                type = Type::Undefined;

                return;
            }

            const std::string extension = Utils::trim(filepath.extension().string());

            if (TYPES.find(extension) == TYPES.end())
            {
                type = Type::Undefined;

                return;
            }

            type = TYPES.at(extension);
        }

        Asset::Asset(const std::string& inFilepath)
        {
            std::string filepath = Utils::trim(inFilepath);

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

        bool Asset::isType(Asset::Type inType) const
        {
            return m_header.type == inType;
        }

        const Asset::Header& Asset::getHeader() const
        {
            return m_header;
        }

        void Asset::setHeader(const Asset::Header& inHeader)
        {
            setFilepath(inHeader.filepath);
            setVersion(inHeader.version);
            setId(inHeader.id);
            setType(inHeader.type);
        }

        const std::filesystem::path& Asset::getFilepath() const
        {
            return m_header.filepath;
        }

        void Asset::setFilepath(const std::filesystem::path& inFilepath)
        {
            m_header.filepath = Utils::trim(inFilepath.string());
        }

        std::uint32_t Asset::getVersion() const
        {
            return m_header.version;
        }

        void Asset::setVersion(std::uint32_t inVersion)
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

        const std::string& Asset::getId() const
        {
            return m_header.id;
        }

        void Asset::setId(const std::string& inId)
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

        Asset::Type Asset::getType() const
        {
            return m_header.type;
        }

        void Asset::setType(Asset::Type inType)
        {
            m_header.type = inType;
        }

        bool Asset::isXMLEmpty()
        {
            pugi::xml_node root = getXMLRoot();

            if (XML::isEmpty(root))
            {
                return true;
            }

            return root.first_child() == root.last_child() && XML::isEmpty(root.first_child());
        }

        pugi::xml_node Asset::getXMLRoot()
        {
            return m_xml.first_child();
        }

        void Asset::createXML(const std::string& inFilepath)
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

        void Asset::fetchXML(const std::string& inFilepath)
        {
            XML::load(m_xml, inFilepath);

            pugi::xml_node root = getXMLRoot();

            bool bIsRoot  = root.parent() == root.root();
            bool bIsAlone = bIsRoot && root.next_sibling() == nullptr;

            if (!bIsRoot || !bIsAlone)
            {
                throw std::runtime_error("Asset files root element must not have any siblings");
            }

            if (!Utils::areEquals(TAG, root.name()))
            {
                throw std::runtime_error("Asset files root element must be a " + TAG);
            }
        }

        void Asset::saveXML()
        {
            XML::save(m_xml, getFilepath().string());
        }

        void Asset::fetchHeader(const std::string& inFilepath)
        {
            if (inFilepath.empty())
            {
                return;
            }

            m_header = Header::fromXML(inFilepath, m_xml);
        }
    }
}