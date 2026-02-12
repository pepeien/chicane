#include "Chicane/Box/Texture/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        TextureManager::TextureManager()
            : Manager()
        {}

        void TextureManager::onLoad(const String& inId, const Texture& inData)
        {
            add(inId, Image(inData.getData(), inData.getVendor()));
        }
    }
}