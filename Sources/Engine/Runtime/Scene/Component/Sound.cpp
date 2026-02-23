#include "Chicane/Runtime/Scene/Component/Sound.hpp"

#include "Chicane/Box.hpp"
#include "Chicane/Box/Sound.hpp"

namespace Chicane
{
    CSound::CSound()
        : Component(),
          m_instance()
    {}

    void CSound::load(const FileSystem::Path& inFilePath)
    {
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