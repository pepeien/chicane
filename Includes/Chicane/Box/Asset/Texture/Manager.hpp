#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"
#include "Chicane/Box/Asset/Texture/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            class CHICANE_BOX Manager : public Box::Manager::Instance<Image::RawData, Image::CompiledData>
            {
            public:
                Manager();
                ~Manager();

            protected:
                void onActivation(const std::string& inId) override;
                void onDeactivation(const std::string& inId) override;

            public:
                // Setup
                void load(const std::string& inId, const Texture::Instance* inTexture);

                // Data
                const Image::RawData& getInstance(const std::string& inId) const;
                const Image::CompiledData& getData(const std::string& inId) const;
                std::uint32_t getIndex(const std::string& inId) const;
            };
        }
    }
}