#include "Chicane/Box/Sound/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        static const Sound::Raw EMPTY_RAW_DATA = {};

        SoundManager::SoundManager()
            : Manager()
        {}

        void SoundManager::onLoad(const String& inId, const Sound& inData)
        {
            add(inId, inData.getData());
        }
    }
}