#include "Chicane/Box/Sound.hpp"

#include "Chicane/Box/Asset/Preview.hpp"

#include "Chicane/Core/Base64.hpp"

namespace Chicane
{
    namespace Box
    {
        Sound::Sound(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_data({})
        {
            fetchDataFromXML();
        }

        const Sound::Raw& Sound::getData() const
        {
            return m_data;
        }

        void Sound::setData(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                throw std::runtime_error("Sound source file was not found");
            }

            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Sound::setData(const Raw& inData)
        {
            if (!setPayload(Base64::encode(inData)))
            {
                throw std::runtime_error("Failed to save the sound [" + m_header.filepath.toString() + "] data");
            }

            m_data = inData;
            bakePreview();
        }

        void Sound::fetchDataFromXML()
        {
            if (isXMLEmpty())
            {
                return;
            }

            m_data = Base64::decodeToUnsigned(getPayload());
        }

        void Sound::bakePreview()
        {
            if (getFilepath().isEmpty() || m_data.empty())
            {
                return;
            }

            const std::unique_ptr<AssetPreview> preview =
                AssetPreview::createFromSound(getFilepath(), getId(), m_data);
            if (!preview || !preview->image)
            {
                return;
            }

            AssetPreview::write(getXML(), getId(), AssetType::Sound, *preview->image);
        }
    }
}