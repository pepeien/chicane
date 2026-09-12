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
              m_bNormal(false),
              m_data(std::make_shared<Image>()),
              m_frames({}),
              m_delays({}),
              m_chains({})
        {
            fetchVendorFromXML();
            fetchDataFromXML();
        }

        bool Texture::isEmpty() const
        {
            if (m_data && m_data->getPixels() != nullptr && m_data->getWidth() > 0 && m_data->getHeight() > 0)
            {
                return false;
            }

            const std::shared_ptr<Image::MipChain> chain = getMipChain(0);

            return !chain || chain->isEmpty();
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
            bakeMips();
        }

        bool Texture::isNormal() const
        {
            return m_bNormal;
        }

        void Texture::setNormal(bool inValue)
        {
            m_bNormal = inValue;
            setAttribute(NORMAL_ATTRIBUTE_NAME, inValue ? "true" : "false");
        }

        std::shared_ptr<Image::MipChain> Texture::getMipChain(std::size_t inIndex) const
        {
            if (m_chains.empty())
            {
                return {};
            }

            return m_chains.at(std::min(inIndex, m_chains.size() - 1));
        }

        bool Texture::bakeMips()
        {
            if (isAnimated())
            {
                if (m_chains.empty())
                {
                    rebuildMipChains();
                }

                return false;
            }

            Image::Instance source;
            if (!m_chains.empty() && m_chains.front())
            {
                source = m_chains.front()->decode(0);
            }
            if (!source)
            {
                source = m_data;
            }

            if (!source || source->getPixels() == nullptr || source->getWidth() <= 0 || source->getHeight() <= 0)
            {
                return false;
            }

            auto chain = std::make_shared<Image::MipChain>(
                Image::makeMipChain(*source, Image::MAX_SIZE, m_bNormal || looksLikeNormal())
            );
            if (chain->isEmpty())
            {
                return false;
            }

            chain->ensureDecoded(chain->streamTailMinMip());
            m_chains = {chain};
            writeMipsToXML(*chain);
            setVersion(FORMAT_VERSION);

            return true;
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

            m_vendor  = Image::parseVendor(getAttribute(VENDOR_ATTRIBUTE_NAME).as_string());
            m_bNormal = String(getAttribute(NORMAL_ATTRIBUTE_NAME).as_string()).toLower().equals("true") ||
                        looksLikeNormal();
        }

        void Texture::fetchDataFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            if (fetchMipsFromXML())
            {
                rebuildFrames();

                return;
            }

            const String payload = getPayload();
            if (!payload.isEmpty())
            {
                m_data.reset(new Image(Base64::decodeToUnsigned(payload), m_vendor));
            }

            rebuildFrames();
            rebuildMipChains();
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

        void Texture::rebuildMipChains()
        {
            m_chains.clear();

            const bool bNormal = m_bNormal || looksLikeNormal();
            if (m_frames.empty() && m_data)
            {
                m_chains.push_back(
                    std::make_shared<Image::MipChain>(Image::makeMipChain(*m_data, Image::MAX_SIZE, bNormal))
                );
                m_chains.back()->ensureDecoded(m_chains.back()->streamTailMinMip());

                return;
            }

            for (const Image::Instance& frame : m_frames)
            {
                if (!frame)
                {
                    m_chains.push_back({});

                    continue;
                }

                auto chain = std::make_shared<Image::MipChain>(Image::makeMipChain(*frame, Image::MAX_SIZE, bNormal));
                chain->ensureDecoded(chain->streamTailMinMip());
                m_chains.push_back(std::move(chain));
            }
        }

        void Texture::bakePreview()
        {
            if (isEmpty() || getFilepath().isEmpty() || !m_data)
            {
                return;
            }

            AssetPreview::write(getXML(), AssetType::Texture, *m_data);
        }

        void Texture::writeMipsToXML(const Image::MipChain& inChain)
        {
            pugi::xml_node root = getXML();
            if (root.empty() || inChain.isEmpty())
            {
                return;
            }

            setAttribute(WIDTH_ATTRIBUTE_NAME, String(std::to_string(inChain.getWidth())));
            setAttribute(HEIGHT_ATTRIBUTE_NAME, String(std::to_string(inChain.getHeight())));
            setAttribute(MIPS_ATTRIBUTE_NAME, String(std::to_string(inChain.getCount())));
            if (m_bNormal || looksLikeNormal())
            {
                setNormal(true);
            }

            pugi::xml_node value = root.child(VALUE_TAG);
            if (!value.empty())
            {
                root.remove_child(value);
            }

            pugi::xml_node mips = root.child(MIPS_TAG);
            if (!mips.empty())
            {
                root.remove_child(mips);
            }

            mips = root.append_child(MIPS_TAG);
            for (std::uint32_t level = 0; level < inChain.getCount(); level++)
            {
                const Image::Mip& mip = inChain.levels[level];
                if (mip.encoded.empty())
                {
                    continue;
                }

                pugi::xml_node node = mips.append_child(MIP_TAG);
                node.append_attribute(LEVEL_ATTRIBUTE_NAME).set_value(level);
                const String encoded = Base64::encode(mip.encoded);
                node.text().set(encoded.toChar(), encoded.size());
            }
        }

        bool Texture::fetchMipsFromXML()
        {
            pugi::xml_node root = getXML();
            if (root.empty())
            {
                return false;
            }

            pugi::xml_node mips = root.child(MIPS_TAG);
            if (mips.empty())
            {
                return false;
            }

            auto chain     = std::make_shared<Image::MipChain>();
            chain->bNormal = m_bNormal || looksLikeNormal();

            std::uint32_t width  = getAttribute(WIDTH_ATTRIBUTE_NAME).as_uint();
            std::uint32_t height = getAttribute(HEIGHT_ATTRIBUTE_NAME).as_uint();
            std::uint32_t count  = getAttribute(MIPS_ATTRIBUTE_NAME).as_uint();

            for (pugi::xml_node node = mips.child(MIP_TAG); node; node = node.next_sibling(MIP_TAG))
            {
                const std::uint32_t level = node.attribute(LEVEL_ATTRIBUTE_NAME).as_uint();
                if (chain->levels.size() <= level)
                {
                    chain->levels.resize(level + 1);
                }

                Image::Mip& mip = chain->levels[level];
                mip.encoded     = Base64::decodeToUnsigned(node.text().as_string());
            }

            if (chain->levels.empty())
            {
                return false;
            }

            if (count == 0)
            {
                count = static_cast<std::uint32_t>(chain->levels.size());
            }

            if (width == 0 || height == 0)
            {
                for (std::uint32_t level = 0; level < chain->levels.size(); level++)
                {
                    if (chain->levels[level].encoded.empty())
                    {
                        continue;
                    }

                    const Image::Instance decoded = chain->decode(level);
                    if (!decoded)
                    {
                        continue;
                    }

                    width  = Image::mipDimension(static_cast<std::uint32_t>(decoded->getWidth()), 0) << level;
                    height = Image::mipDimension(static_cast<std::uint32_t>(decoded->getHeight()), 0) << level;

                    break;
                }
            }

            if (chain->levels.size() < count)
            {
                chain->levels.resize(count);
            }

            for (std::uint32_t level = 0; level < chain->levels.size(); level++)
            {
                Image::Mip& mip = chain->levels[level];
                mip.width       = static_cast<int>(Image::mipDimension(width, level));
                mip.height      = static_cast<int>(Image::mipDimension(height, level));
            }

            chain->ensureDecoded(chain->streamTailMinMip());
            m_chains = {chain};

            if (Image::Instance tail = chain->decode(chain->streamTailMinMip()))
            {
                m_data = tail;
            }
            else if (Image::Instance top = chain->decode(0))
            {
                m_data = top;
            }

            return !chain->isEmpty();
        }

        bool Texture::looksLikeNormal() const
        {
            const String id   = getId().toLower();
            const String path = getFilepath().toString().toLower();

            return id.contains("normal") || id.contains("bump") || path.contains("normal") || path.contains("bump");
        }
    }
}
