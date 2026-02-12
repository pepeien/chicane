#include "Chicane/Box/Texture.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        Texture::Texture(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_vendor(ImageVendor::Undefined)
        {
            fetchVendor();
            fetchData();
        }

        ImageVendor Texture::getVendor() const
        {
            return m_vendor;
        }

        void Texture::setVendor(ImageVendor inVendor)
        {
            m_vendor = inVendor;

            String vendorValue = Image::extractVendor(m_vendor);

            pugi::xml_node root = getXML();

            if (root.attribute(VENDOR_ATTRIBUTE_NAME).empty())
            {
                root.append_attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorValue.toChar());

                return;
            }

            root.attribute(VENDOR_ATTRIBUTE_NAME).set_value(vendorValue.toChar());
        }

        const Image::Raw& Texture::getData() const
        {
            return m_data;
        }

        void Texture::setData(const String& inFilepath)
        {
            if (!FileSystem::exists(inFilepath.toStandard()))
            {
                m_data.clear();

                return;
            }

            setData(FileSystem::readUnsigned(inFilepath.toStandard()));
        }

        void Texture::setData(const Image::Raw& inData)
        {
            m_data = inData;

            getXML().text().set(Base64::encode(inData));
        }

        void Texture::fetchVendor()
        {
            const String value = Xml::getAttribute(VENDOR_ATTRIBUTE_NAME, getXML()).as_string();
            setVendor(Image::extractVendor(value));
        }

        void Texture::fetchData()
        {
            const String& value = getXML().text().as_string();
            setData(Base64::decodeToUnsigned(value));
        }
    }
}