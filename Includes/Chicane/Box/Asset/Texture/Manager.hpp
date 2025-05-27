#pragma once

#include "Chicane/Box/Asset/Texture/Instance.hpp"
#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            class CHICANE_BOX Manager : public Box::Manager::Instance<Image::Raw, Image::Instance>
            {
            public:
                Manager();
                virtual ~Manager() = default;

            protected:
                void onActivation(const std::string& inId) override;
                void onDeactivation(const std::string& inId) override;

            public:
                // Setup
                void load(const std::string& inId, const Texture::Instance& inAsset);

                // Data
                const Image::Raw& getInstance(const std::string& inId) const;
                const Image::Instance& getData(const std::string& inId) const;
                std::uint32_t getIndex(const std::string& inId) const;
            };
        }
    }
}