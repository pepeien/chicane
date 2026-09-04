#include "Chicane/Box/Texture.hpp"

#include <algorithm>

#include "Chicane/Box/Asset/Preview.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        const Texture* Texture::getDefault()
        {
            return Box::load<Texture>(DEFAULT_SOURCE);
        }

        Texture::Texture(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_vendor(ImageVendor::Undefined),
              m_data(std::make_shared<Image>()),
              m_frames({}),
              m_delays({})
        {
            fetchVendorFromXML();
            fetchDataFromXML();
        }

        bool Texture::isEmpty() const
        {
            return !m_data || m_data->getPixels() == nullptr || m_data->getWidth() <= 0 || m_data->getHeight() <= 0;
        }

        bool Texture::isAnimated() const
        {
            return getFrameCount() > 1;
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

        Image::Reference Texture::getData() const
        {
            return m_data;
        }

        void Texture::setData(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                throw std::runtime_error("Texture source file was not found");
            }

            setVendor(inFilepath.extension().toString());
            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Texture::setData(const Image::Raw& inData)
        {
            if (!setPayload(Base64::encode(inData)))
            {
                throw std::runtime_error("Failed to save the texture [" + m_header.filepath.toString() + "] data");
            }

            m_data.reset(new Image(inData, m_vendor));
            rebuildFrames();
            bakePreview();
        }

        std::size_t Texture::getFrameCount() const
        {
            return m_frames.empty() ? 1 : m_frames.size();
        }

        int Texture::getDelay(std::size_t inIndex) const
        {
            if (m_delays.empty())
            {
                return 0;
            }

            const int delay = m_delays.at(std::min(inIndex, m_delays.size() - 1));

            return delay < 20 ? 100 : delay;
        }

        Image::Reference Texture::getFrame(std::size_t inIndex) const
        {
            if (m_frames.empty())
            {
                return m_data;
            }

            return m_frames.at(std::min(inIndex, m_frames.size() - 1));
        }

        String Texture::getFrameId(std::size_t inIndex) const
        {
            if (inIndex == 0)
            {
                return getId();
            }

            String result = getId();
            result.append(":");
            result.append(String(std::to_string(inIndex)));

            return result;
        }

        void Texture::fetchVendorFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            m_vendor = Image::parseVendor(getAttribute(VENDOR_ATTRIBUTE_NAME).as_string());
        }

        void Texture::fetchDataFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            m_data.reset(new Image(Base64::decodeToUnsigned(getPayload()), m_vendor));
            rebuildFrames();
        }

        void Texture::rebuildFrames()
        {
            m_frames.clear();
            m_delays.clear();

            if (!m_data)
            {
                return;
            }

            const int count = m_data->getFrameCount();
            if (count <= 1)
            {
                m_frames.push_back(m_data);
                m_delays.push_back(m_data->getDelay(0));

                return;
            }

            for (int i = 0; i < count; i++)
            {
                m_frames.push_back(
                    std::make_shared<Image>(
                        m_data->getPixels(i),
                        m_data->getWidth(),
                        m_data->getHeight(),
                        m_data->getChannel(),
                        m_data->getFormat()
                    )
                );
                m_delays.push_back(m_data->getDelay(i));
            }

            m_data = m_frames.front();
        }

        void Texture::bakePreview()
        {
            if (isEmpty() || getFilepath().isEmpty() || !m_data)
            {
                return;
            }

            AssetPreview::write(getXML(), getId(), AssetType::Texture, *m_data);
        }
    }
}
