#include "Chicane/Runtime/Scene/Component/Sound.reflected.hpp"

#include "Chicane/Box.hpp"
#include "Chicane/Box/Sound.hpp"

namespace Chicane
{
    CSound::CSound()
        : Component(),
          sound(),
          m_instance()
    {}

    void CSound::onPropertyEdited(const String& inName)
    {
        if (inName.equals(SOUND_ATTRIBUTE_NAME))
        {
            load(sound);
        }
    }

    void CSound::load(const FileSystem::Path& inFilePath)
    {
        sound = inFilePath;

        if (inFilePath.isEmpty())
        {
            return;
        }

        if (const Box::Sound* asset = Box::load<Box::Sound>(inFilePath))
        {
            m_instance.setData(asset->getData());
        }
    }

    void CSound::play()
    {
        m_instance.play();
    }
}