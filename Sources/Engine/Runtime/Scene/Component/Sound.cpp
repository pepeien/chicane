#include "Chicane/Runtime/Scene/Component/Sound.hpp"

#include "Chicane/Box/Sound/Manager.hpp"

namespace Chicane
{
    CSound::CSound()
        : Component(),
          m_sound({})
    {}

    void CSound::load(const FileSystem::Path& inFilePath)
    {
        if (inFilePath.empty())
        {
            return;
        }

        m_sound.setData(Box::load<Box::Sound>(inFilePath)->getData());
    }

    void CSound::play()
    {
        m_sound.play();
    }
}