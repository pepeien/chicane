#include "Chicane/Box/Asset/Texture.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        static constexpr auto VENDOR_ATTRIBUTE_NAME = "vendor";

        const std::unordered_map<std::string, Chicane::Texture::Vendor> VENDOR_MAP {
            { "PNG", Chicane::Texture::Vendor::Png }
        };

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

        void Texture::setVendor(Chicane::Texture::Vendor inVendor)
        {
            m_vendor = inVendor;

            auto vendor = std::find_if(
                VENDOR_MAP.begin(),
                VENDOR_MAP.end(),
                [inVendor](const auto& inPair) { return inPair.second == inVendor; }
            );

            if (vendor == VENDOR_MAP.end())
            {
                return;
            }

            std::string vendorID = vendor->first;

            pugi::xml_node root = getXMLRoot();

            if (root.attribute(VENDOR_ATTRIBUTE_NAME).empty())
            {
                root.append_attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.c_str());

                return;
            }

            root.attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.c_str());
        }

        const std::vector<unsigned char>& Texture::getData() const
        {
            return m_data;
        }

        void Texture::setData(const std::vector<unsigned char>& inData)
        {
            if (inData.empty())
            {
                return;
            }

            m_data = inData;

            getXMLRoot().text().set(base64::encode(inData));
        }

        void Texture::setData(const std::string& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                return;
            }

            setData(base64::read_file(inFilepath));
        }

        void Texture::fetchVendor()
        {
            if (getFilepath().empty() || isXMLEmpty())
            {
                return;
            }

            std::string vendor = XML::getAttribute(
                VENDOR_ATTRIBUTE_NAME,
                getXMLRoot()
            ).as_string();

            if (VENDOR_MAP.find(vendor) == VENDOR_MAP.end())
            {
                m_vendor = Chicane::Texture::Vendor::Undefined;

                return;
            }

            m_vendor = VENDOR_MAP.at(vendor);
        }

        void Texture::fetchData()
        {
            if (getFilepath().empty() || isXMLEmpty())
            {
                return;
            }

            m_data = base64::decode(getXMLRoot().text().as_string());
        }
    }
}