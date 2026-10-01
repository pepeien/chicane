#include "Chicane/Runtime/Scene/Actor/Sound.reflected.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ASound::ASound()
        : Actor(),
          m_sound(nullptr)
    {}

    void ASound::createDefaultComponents()
    {
        m_sound = createDefaultComponent<CSound>("Sound");
    }

    void ASound::load(const FileSystem::Path& inFilePath)
    {
        if (!m_sound)
        {
            return;
        }

        m_sound->load(inFilePath);
    }

    void ASound::play() const
    {
        if (!m_sound)
        {
            return;
        }

        m_sound->play();
    }
}