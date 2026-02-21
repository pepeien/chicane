#include "Chicane/Box/Texture.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        Texture::Texture(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_vendor(ImageVendor::Undefined),
              m_data({})
        {
            fetchVendor();
            fetchData();
        }

        ImageVendor Texture::getVendor() const
        {
            return m_vendor;
        }

        void Texture::setVendor(const String& inValue)
        {
            setVendor(Image::parseVendor(inValue));
        }

        void Texture::setVendor(ImageVendor inValue)
        {
            m_vendor = inValue;

            setAttribute(VENDOR_ATTRIBUTE_NAME, Image::getVendorExtension(m_vendor));
        }

        const Image::Raw& Texture::getData() const
        {
            return m_data;
        }

        void Texture::setData(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                throw std::runtime_error("Texture source file was not found");
            }

            setVendor(inFilepath.extension().string());
            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Texture::setData(const Image::Raw& inData)
        {
            m_data = inData;

            if (!getXML().text().set(Base64::encode(m_data).toChar()))
            {
                throw std::runtime_error("Failed to save the texture [" + m_header.filepath.string() + "] data");
            }
        }

        void Texture::fetchVendor()
        {
            if (isEmpty())
            {
                return;
            }

            m_vendor = Image::parseVendor(getAttribute(VENDOR_ATTRIBUTE_NAME).as_string());
        }

        void Texture::fetchData()
        {
            if (isEmpty())
            {
                return;
            }

            m_data = Base64::decodeToUnsigned(getXML().text().as_string());
        }
    }
}