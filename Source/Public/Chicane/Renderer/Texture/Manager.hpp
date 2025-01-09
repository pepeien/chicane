#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Asset/Texture.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Renderer/Manager.hpp"

namespace Chicane
{
    namespace Texture
    {
        class Manager : public Chicane::Manager::Instance<Image::Instance, Image::Data>
        {
        public:
            Manager();
            ~Manager();

        protected:
            void onActivation(const std::string& inId) override;
            void onDeactivation(const std::string& inId) override;

        public:
            // Setup
            void load(const std::string& inId, const Box::Texture* inAsset);

            // Data
            const Image::Instance& getInstance(const std::string& inId) const;
            const Image::Data& getData(const std::string& inId) const;
            std::uint32_t getIndex(const std::string& inId) const;
        };
    }
}