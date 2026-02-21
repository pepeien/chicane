#include "Chicane/Box/Sound.hpp"

#include "Chicane/Core/Base64.hpp"

namespace Chicane
{
    namespace Box
    {
        Sound::Sound(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_data({})
        {
            fetchData();
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
            m_data = inData;

            if (!getXML().text().set(Base64::encode(m_data).toChar()))
            {
                throw std::runtime_error("Failed to save the sound [" + m_header.filepath.string() + "] data");
            }
        }

        void Sound::fetchData()
        {
            if (isEmpty())
            {
                return;
            }

            m_data = Base64::decodeToUnsigned(getXML().text().as_string());
        }
    }
}