#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Image.hpp"

#include "Chicane/Box/Asset/Texture/Instance.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            class Manager : public Box::Manager::Instance<Image::Instance, Image::Data>
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
                const Image::Instance& getInstance(const std::string& inId) const;
                const Image::Data& getData(const std::string& inId) const;
                std::uint32_t getIndex(const std::string& inId) const;
            };
        }
    }
}