#include "Chicane/Box/Sound/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        SoundManager::SoundManager()
            : Manager()
        {}

        void SoundManager::onLoad(const String& inId, const Sound& inData)
        {
            add(inId, inData.getData());
        }
    }
}