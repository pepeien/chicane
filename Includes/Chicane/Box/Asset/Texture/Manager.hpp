#pragma once

#include "Chicane/Box/Asset/Texture.hpp"
#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX TextureManager : public Box::Manager<Image::Raw, Image>
        {
        public:
            TextureManager();
            virtual ~TextureManager() = default;

        protected:
            void onActivation(const String& inId) override;
            void onDeactivation(const String& inId) override;

        public:
            // Setup
            void load(const String& inId, const Texture& inAsset);

            // Data
            const Image::Raw& getInstance(const String& inId) const;
            const Image& getData(const String& inId) const;
            std::uint32_t getIndex(const String& inId) const;
        };
    }
}