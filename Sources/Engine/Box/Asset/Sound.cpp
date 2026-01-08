#include "Chicane/Box/Asset/Sound.hpp"

#include "Chicane/Core/Base64.hpp"

namespace Chicane
{
    namespace Box
    {
        Sound::Sound(const FileSystem::Path& inFilepath)
            : Asset(inFilepath)
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
                return;
            }

            setData(FileSystem::readUnsigned(inFilepath));
        }

        void Sound::setData(const Raw& inData)
        {
            if (inData.empty())
            {
                return;
            }

            m_data = inData;

            getXML().text().set(Base64::encode(inData));
        }

        void Sound::fetchData()
        {
            if (getFilepath().empty() || isEmpty())
            {
                return;
            }

            m_data = Base64::decodeToUnsigned(getXML().text().as_string());
        }
    }
}