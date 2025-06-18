#include "Chicane/Box/Asset/Texture.hpp"

namespace Chicane
{
    namespace Box
    {
        static const std::unordered_map<String, Image::Vendor> VENDOR_MAP {
            { "PNG", Image::Vendor::Png }
        };

        Texture::Texture(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
            m_vendor(Image::Vendor::Undefined)
        {
            fetchVendor();
            fetchData();
        }

        Image::Vendor Texture::getVendor() const
        {
            return m_vendor;
        }

        void Texture::setVendor(Image::Vendor inVendor)
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

            String vendorID = vendor->first;

            pugi::xml_node root = getXML();

            if (root.attribute(VENDOR_ATTRIBUTE_NAME).empty())
            {
                root.append_attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.toChar());

                return;
            }

            root.attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorID.toChar());
        }

        const Image::Raw& Texture::getData() const
        {
            return m_data;
        }

        void Texture::setData(const String& inFilepath)
        {
            if (!FileSystem::exists(inFilepath.toStandard()))
            {
                return;
            }

            setData(FileSystem::readUnsigned(inFilepath.toStandard()));
        }

        void Texture::setData(const Image::Raw& inData)
        {
            if (inData.empty())
            {
                return;
            }

            m_data = inData;

            getXML().text().set(Base64::encode(inData));
        }

        void Texture::fetchVendor()
        {
            if (getFilepath().empty() || isEmpty())
            {
                return;
            }

            String vendor = Xml::getAttribute(
                VENDOR_ATTRIBUTE_NAME,
                getXML()
            ).as_string();

            if (VENDOR_MAP.find(vendor) == VENDOR_MAP.end())
            {
                m_vendor = Image::Vendor::Undefined;

                return;
            }

            m_vendor = VENDOR_MAP.at(vendor);
        }

        void Texture::fetchData()
        {
            if (getFilepath().empty() || isEmpty())
            {
                return;
            }

            m_data = Base64::decodeToUnsigned(getXML().text().as_string());
        }
    }
}