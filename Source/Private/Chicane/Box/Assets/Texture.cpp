#include "Chicane/Box/Assets/Texture.hpp"

#include "Chicane/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        constexpr auto VENDOR_ATTRIBUTE_NAME = "vendor";

        Texture::Texture(const std::string& inFilepath)
            : Asset(inFilepath),
            m_vendor(Chicane::Texture::Vendor::Undefined)
        {
            fetchVendor();
            fetchData();
        }

        Chicane::Texture::Vendor Texture::getVendor() const
        {
            return m_vendor;
        }

        const std::vector<unsigned char>& Texture::getData() const
        {
            return m_data;
        }

        void Texture::fetchVendor()
        {
            if (m_header.filepath.empty() || m_xml.empty())
            {
                return;
            }

            std::uint8_t vendor = XML::getAttribute(
                VENDOR_ATTRIBUTE_NAME,
                m_xml.first_child()
            ).as_uint();

            m_vendor = (Chicane::Texture::Vendor) vendor;
        }

        void Texture::fetchData()
        {
            if (m_header.filepath.empty() || m_xml.empty())
            {
                return;
            }

            auto current = m_xml.first_child().child_value();

            if (!current)
            {
                return;
            }

            while (*current != '\0')
            {
                m_data.push_back(*current);

                current++;
            }
        }
    }
}