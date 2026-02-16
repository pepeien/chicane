#include "Chicane/Runtime/Scene/Actor/Sound.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ASound::ASound()
        : Actor(),
          m_sound(nullptr)
    {}

    void ASound::onLoad()
    {
        m_sound = getScene()->createComponent<CSound>();
        m_sound->attachTo(this);
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