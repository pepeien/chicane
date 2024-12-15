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

            auto currentChar = m_xml.first_child().child_value();

            if (!currentChar)
            {
                return;
            }

            std::string encodedBase64 = "";

            while (*currentChar != '\0')
            {
                encodedBase64.push_back(*currentChar);

                currentChar++;
            }

            std::string decodedBase64 = base64_decode(encodedBase64, true);

            m_data.insert(m_data.begin(), decodedBase64.begin(), decodedBase64.end());
        }
    }
}