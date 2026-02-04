#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Texture.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX TextureManager : public Box::Manager<Texture, Image>
        {
        public:
            TextureManager();

        protected:
            void onLoad(const String& inId, const Texture& inData) override;
        };
    }
}