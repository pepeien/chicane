#include "Chicane/Box/Asset.hpp"

#include "Chicane/Box/Assets/Cubemap.hpp"
#include "Chicane/Box/Assets/Mesh.hpp"
#include "Chicane/Box/Assets/Model.hpp"
#include "Chicane/Box/Assets/Texture.hpp"
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
        std::string Asset::TAG             = "Asset";
        std::string Asset::CURRENT_VERSION = "1.0";

        constexpr auto VERSION_ATTRIBUTE_NAME = "version";
        constexpr auto ID_ATTRIBUTE_NAME      = "id";

        Asset::Header Asset::Header::fromFilepath(const std::string& inFilepath)
        {
            if (inFilepath.empty())
            {
                return {};
            }

            pugi::xml_document xml;

            if (!xml.load_file(inFilepath.c_str(), pugi::parse_default | pugi::parse_fragment))
            {
                throw std::runtime_error("Failed to read " + inFilepath);
            }

            if (xml.empty() || xml.children().empty())
            {
                throw std::runtime_error("The asset " + inFilepath + " is invalid");
            }

            return Header::fromXml(inFilepath, xml);
        }

        Asset::Header Asset::Header::fromXml(const std::string& inFilepath, const pugi::xml_document& inDocument)
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

            version = XML::getAttribute(VERSION_ATTRIBUTE_NAME, inRoot).as_string();

            if (!version.empty())
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

            fetchXML(filepath);
            fetchHeader(filepath);
        }

        const Asset::Header& Asset::getHeader() const
        {
            return m_header;
        }

        void Asset::fetchXML(const std::string& inFilepath)
        {
            if (inFilepath.empty())
            {
                return;
            }

            if (!m_xml.load_file(inFilepath.c_str()))
            {
                throw std::runtime_error("Failed to read " + inFilepath);
            }

            if (m_xml.empty() || m_xml.children().empty())
            {
                throw std::runtime_error("The asset " + inFilepath + " is invalid");
            }

            if (m_xml.empty())
            {
                throw std::runtime_error("Document not found");
            }

            pugi::xml_node root = m_xml.first_child();

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

        void Asset::fetchHeader(const std::string& inFilepath)
        {
            if (inFilepath.empty())
            {
                return;
            }

            m_header = Header::fromXml(inFilepath, m_xml);
        }
    }
}