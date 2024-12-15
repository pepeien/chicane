#include "Chicane/Box/Asset.hpp"

#include "Chicane/Box/Assets/Mesh.hpp"
#include "Chicane/Box/Assets/Model.hpp"
#include "Chicane/Box/Assets/Texture.hpp"
#include "Chicane/Xml.hpp"

const std::unordered_map<std::string, Chicane::Box::Asset::Type> TYPES {
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

        Asset::Asset(const std::string& inFilepath)
        {
            std::string filepath = Utils::trim(inFilepath);

            if (filepath.empty())
            {
                return;
            }

            m_header.filepath = filepath;

            fetchXML();
            fetchVersion();
            fetchId();
            fetchType();
        }

        const Asset::Header& Asset::getHeader() const
        {
            return m_header;
        }

        void Asset::validateXML()
        {
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

        void Asset::fetchXML()
        {
            if (m_header.filepath.empty())
            {
                return;
            }

            const std::string filepath = m_header.filepath.string();

            if (!m_xml.load_file(filepath.c_str()))
            {
                throw std::runtime_error("Failed to read " + filepath);
            }

            if (m_xml.empty() || m_xml.children().empty())
            {
                throw std::runtime_error("The asset " + filepath + " is invalid");
            }

            validateXML();
        }

        void Asset::fetchVersion()
        {
            if (m_header.filepath.empty())
            {
                m_header.version = CURRENT_VERSION;

                return;
            }

            m_header.version = XML::getAttribute(VERSION_ATTRIBUTE_NAME, m_xml.first_child()).as_string();
        }

        void Asset::fetchId()
        {
            if (m_header.filepath.empty() || m_xml.empty())
            {
                m_header.id = "";

                return;
            }

            m_header.id = XML::getAttribute(ID_ATTRIBUTE_NAME, m_xml.first_child()).as_string();
        }

        void Asset::fetchType()
        {
            if (m_header.filepath.empty())
            {
                m_header.type = Type::Undefined;

                return;
            }

            const std::string extension = Utils::trim(m_header.filepath.extension().string());

            if (TYPES.find(extension) == TYPES.end())
            {
                m_header.type = Type::Undefined;

                return;
            }

            m_header.type = TYPES.at(extension);
        }
    }
}